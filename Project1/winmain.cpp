#include "CrossHairWindow.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "MenuWindow.h"
#include <mutex>
#include "WindowsFuncs.h"


void RedrawCrossHairWindow(std::shared_ptr<CrossHairWindow> pChWindow, std::shared_ptr<MenuWindow> pMenuWindow) {
    
    std::mutex mu;
   do {
       //force a repaint once on load. this will ensure the crosshair is painted and not invisible when the program starts
       RECT r{ 0,0,pChWindow->Width(),pChWindow->Height() };
       RedrawWindow(pChWindow->GetHandle(), &r, nullptr, RDW_INVALIDATE);

        //use a condition variable so we can avoid busy waiting. this will let us know when an event occured
        //in the menuwindow so we can then update the crosshair window
        std::unique_lock<std::mutex> ul(mu);
        pMenuWindow->cond_signalpaint.wait(ul);
   } while (true);
    
}
void CrossHairDragEnabler(std::shared_ptr<MenuWindow> pMenuWindow) {

    HWND hwnd{ FindWindow("crosshair", "Luv's CrossHair") };

    std::mutex mu;
    while (true) {

        std::unique_lock<std::mutex> ul(mu);
        pMenuWindow->cond_signalreposition.wait(ul); //wait until menu notifies us that we want to enable click-drag of crosshair window
        
        static bool enableclickdrag{ true };
  
        if (enableclickdrag) {

            SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_CLIENTEDGE | WS_EX_LAYERED); //set the window styles to enable mouse messages (transparent windows dont send them
                                                                                //and a client edge to let the user know where they can click-drag
            SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_DRAWFRAME); // i think setwindowpos is used to refresh the window after resetting its styles
            enableclickdrag = !enableclickdrag;

        }
        else {

            SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST); //restore original crosshair window styles
            SetWindowPos(hwnd, 0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_DRAWFRAME);
            enableclickdrag = !enableclickdrag;
        }
       
    }
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevinstance, PSTR szCmdLine, int iCmdShow) {
  

    ULONG_PTR gdiplusToken{ 0 };
    if(!WindowsFuncs::InitGDIPlus(gdiplusToken))
        return 0;

    auto pCh{ std::make_shared<CrossHair>() };

    //read the configuration file and set the crosshair values
    pCh->ReadEntries();

    auto ch_window{ std::make_shared<CrossHairWindow>(pCh,300,300, "Luv's CrossHair", "crosshair", hInstance) };
    auto menu_window{ std::make_shared<MenuWindow>(pCh,"Luv's CrossHair Menu", "crosshairmenu", hInstance) };
    
    //create thread so we can have a loop where the menuwindow signals the crosshairwindow to redraw
    //after adjusting a setting in the menu
    std::thread worker(RedrawCrossHairWindow, ch_window, menu_window);
    std::thread worker1(CrossHairDragEnabler, menu_window);

     MSG msg;
     //If no messages are available, the return value is zero.
     while (GetMessage(&msg, nullptr, 0, 0)){


         TranslateMessage(&msg);
         DispatchMessage(&msg);

     }
     if (msg.message == WM_QUIT) {
         worker.~thread();
         worker1.~thread();
     }

    

    Gdiplus::GdiplusShutdown(gdiplusToken);
    return 1;
}