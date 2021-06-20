#include "CrossHairWindow.h"
#include <mutex>
//#include <wingdi.h>



CrossHairWindow* pch_window{ nullptr };

void CrossHairWindow::OnPaint(HDC& hdc) {

	 ch_cross_brush.SetColor(Gdiplus::Color(	
		pCh->GetColor(Key::CrossColor)[0],
		pCh->GetColor(Key::CrossColor)[1],
		pCh->GetColor(Key::CrossColor)[2]));

	 ch_circle_pen.SetColor(Gdiplus::Color(	
		pCh->GetColor(Key::CircleColor)[0],
		pCh->GetColor(Key::CircleColor)[1],
		pCh->GetColor(Key::CircleColor)[2]));

	ch_circle_brush.SetColor(Gdiplus::Color(		
		pCh->GetColor(Key::CircleColor)[0],
		pCh->GetColor(Key::CircleColor)[1],
		pCh->GetColor(Key::CircleColor)[2]));

	ch_dot_brush.SetColor(Gdiplus::Color(	
		 pCh->GetColor(Key::DotColor)[0],
		 pCh->GetColor(Key::DotColor)[1],
		 pCh->GetColor(Key::DotColor)[2]));

	//do all the drawing to our back buffer
	//since we're clearing the backbuffer before drawing onto it, we only present the final image to the window 
	//since we are not clearing the window itself, which causes flickering
	offscreengraphics->Clear(Gdiplus::Color(transpcolor[0], transpcolor[1], transpcolor[2]));


	//form a cross
	//topleftx, toplefty, width, height

	offscreengraphics->FillRectangle(&ch_cross_brush,  (width / 2) - pCh->Get(Key::CrossLength) - pCh->Get(Key::CenterGap), (height / 2) - pCh->Get(Key::Thickness), pCh->Get(Key::CrossLength), pCh->Get(Key::Thickness) * 2); //horiz left
	offscreengraphics->FillRectangle(&ch_cross_brush, (width / 2) + pCh->Get(Key::CenterGap), (height / 2) - pCh->Get(Key::Thickness), pCh->Get(Key::CrossLength), pCh->Get(Key::Thickness) * 2); //horiz right
	offscreengraphics->FillRectangle(&ch_cross_brush,  (width / 2) - pCh->Get(Key::Thickness), ((height / 2) - pCh->Get(Key::CrossLength)) - pCh->Get(Key::CenterGap), pCh->Get(Key::Thickness) * 2, pCh->Get(Key::CrossLength) ); //vert top
	offscreengraphics->FillRectangle(&ch_cross_brush, (width / 2) - pCh->Get(Key::Thickness), (height / 2) + pCh->Get(Key::CenterGap), pCh->Get(Key::Thickness) * 2, pCh->Get(Key::CrossLength)); //ver bottom

	//draw the outline of the cross on top of the filled rectangles (so the filled rects wouldn't overdraw the outline)
	offscreengraphics->DrawRectangle(&ch_cross_pen, (width / 2) - pCh->Get(Key::CrossLength) - pCh->Get(Key::CenterGap), (height / 2) - pCh->Get(Key::Thickness), pCh->Get(Key::CrossLength), pCh->Get(Key::Thickness) * 2); //horiz left
	offscreengraphics->DrawRectangle(&ch_cross_pen, (width / 2) + pCh->Get(Key::CenterGap), (height / 2) - pCh->Get(Key::Thickness), pCh->Get(Key::CrossLength), pCh->Get(Key::Thickness) * 2); //horiz right
	offscreengraphics->DrawRectangle(&ch_cross_pen, (width / 2) - pCh->Get(Key::Thickness), ((height / 2) - pCh->Get(Key::CrossLength)) - pCh->Get(Key::CenterGap), pCh->Get(Key::Thickness) * 2, pCh->Get(Key::CrossLength)); //vert top
	offscreengraphics->DrawRectangle(&ch_cross_pen, (width / 2) - pCh->Get(Key::Thickness), (height / 2) + pCh->Get(Key::CenterGap), pCh->Get(Key::Thickness) * 2, pCh->Get(Key::CrossLength)); //ver bottom

	
	//draw the dot			
	offscreengraphics->FillEllipse(&ch_dot_brush, (width / 2) - pCh->Get(Key::DotSize), (height / 2) - pCh->Get(Key::DotSize), pCh->Get(Key::DotSize) * 2, pCh->Get(Key::DotSize) * 2);
	//draw the circle
	offscreengraphics->DrawEllipse(&ch_circle_pen, (width / 2) - pCh->Get(Key::CircleSize), (height / 2) - pCh->Get(Key::CircleSize), pCh->Get(Key::CircleSize) * 2, pCh->Get(Key::CircleSize) * 2);

	//finally, present the image to the screen
	graphics->DrawImage(backbuffer.get(), 0, 0);
}
LRESULT CALLBACK CrossHairWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static bool dragmainwindow{ false };
	switch (msg)
	{
	case WM_CREATE: {
		break;
	}
	case WM_CLOSE: {
		DestroyWindow(hWnd);
		break;
	}
	case WM_QUIT: {
		break;
	}
	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	}
	case WM_ERASEBKGND: { //prevent the window from erasing the background causing flickering
		return 0;
	}
	case WM_LBUTTONDOWN: {
		if (hWnd == pch_window->hWnd)
			dragmainwindow = true;

		break;
	}
	case WM_LBUTTONUP: {
		if (hWnd == pch_window->hWnd)
			dragmainwindow = false;
		break;
	}
	case WM_MOUSEMOVE: {

		//when dragging is enabled, do this so we can move the crosshair window
		if (hWnd == pch_window->hWnd) {

			POINT mousecoordinates{ LOWORD(lParam), HIWORD(lParam) }; //get the current mouse coordinates in client space, then convert to screen space


			static POINT prev_coordinates{ };
			ClientToScreen(hWnd, &mousecoordinates);                 //convert the client space coordinates to screen space (desktop resolution)

			if (dragmainwindow) {

				RECT mainWindowRect{ 0 };
				GetWindowRect(pch_window->hWnd, &mainWindowRect); //get the windows current position and dimensions in screen-space
				auto movedby{ POINT{ mousecoordinates.x - prev_coordinates.x, mousecoordinates.y - prev_coordinates.y} }; //calculate how much we moved our mouse by since the last move
				MoveWindow(hWnd, mainWindowRect.left + movedby.x, mainWindowRect.top + movedby.y, pch_window->width, pch_window->height, true); //drag the window by the amount we are moving the mouse, without repositioning it to our cursor position

			}
			prev_coordinates = mousecoordinates; //store the current mouse coordinates for future calculations
		}
		break;
	}
	case WM_PAINT: {

		//reset the transparency level	
		pch_window->MakeTransparent(hWnd, LWA_COLORKEY | LWA_ALPHA, RGB(pch_window->transpcolor[0], pch_window->transpcolor[1], pch_window->transpcolor[2]), pch_window->pCh->Get(Key::Transparency));

		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);
		pch_window->OnPaint(ps.hdc);
		EndPaint(hWnd, &ps);
		break;

	}

	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
CrossHairWindow::CrossHairWindow(std::shared_ptr<CrossHair> pCh, int32_t width, int32_t height, const std::string& title, const std::string& classname, HINSTANCE hInstance)
	:
	hInstance(hInstance),
	windowtitle(title),
	classname(classname),
	pCh(pCh),
	width(width),
	height(height),
	ch_cross_pen(Gdiplus::Color(
		1,  //r
		1,  //g
		1), //b
		0.5f),
ch_cross_brush(Gdiplus::Color(
	pCh->GetColor(Key::CrossColor)[0],
	pCh->GetColor(Key::CrossColor)[1],
	pCh->GetColor(Key::CrossColor)[2])),
ch_circle_pen(Gdiplus::Color(
	pCh->GetColor(Key::CircleColor)[0],
	pCh->GetColor(Key::CircleColor)[1],
	pCh->GetColor(Key::CircleColor)[2]),
	pCh->Get(Key::Thickness) / 2),
ch_circle_brush(Gdiplus::Color(
	pCh->GetColor(Key::CircleColor)[0],
	pCh->GetColor(Key::CircleColor)[1],
	pCh->GetColor(Key::CircleColor)[2])),
ch_dot_brush(Gdiplus::Color(
	pCh->GetColor(Key::DotColor)[0],
	pCh->GetColor(Key::DotColor)[1],
	pCh->GetColor(Key::DotColor)[2]))

{
	
	::pch_window = this;

	//init window class
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = classname.c_str();
	wc.hbrBackground = CreateSolidBrush(RGB(transpcolor[0],	transpcolor[1],	transpcolor[2]));

	RegisterClass(&wc);

	//get the desktop resolution
	static long screen_width{ 0 };
	static long screen_height{ 0 };
	static std::once_flag flag;

	{
		std::call_once(flag, []() {
			//get desktop resolution once
			auto hdesktop{ GetDesktopWindow() };
			RECT desktopres{};
			GetWindowRect(hdesktop, &desktopres);
			screen_width = desktopres.right;
			screen_height = desktopres.bottom;
			});
	}
	

	hWnd = CreateWindowEx( WS_EX_TRANSPARENT| WS_EX_LAYERED | WS_EX_TOPMOST,
		classname.c_str(),
		windowtitle.c_str(),

		WS_POPUP,
		(screen_width / 2) - width / 2,
		(screen_height / 2) - height / 2,
		width,
		height,
		nullptr,
		nullptr,
		hInstance,
		NULL);

	ShowWindow(hWnd, SW_SHOW);

	graphics = std::make_unique<Gdiplus::Graphics>(GetDC(hWnd));
	backbuffer = std::make_unique<Gdiplus::Bitmap>(width, height);
	offscreengraphics = std::make_unique<Gdiplus::Graphics>(backbuffer.get());

	//make the background of our window transparent
	MakeTransparent(hWnd, LWA_COLORKEY | LWA_ALPHA,RGB(transpcolor[0], transpcolor[1], transpcolor[2]) , pch_window->pCh->Get(Key::Transparency));


}
CrossHairWindow::~CrossHairWindow()
{	
	UnregisterClass(classname.c_str(), hInstance);
}
