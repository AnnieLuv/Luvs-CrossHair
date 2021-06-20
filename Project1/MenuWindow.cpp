#include "MenuWindow.h"
#include "CommCtrl.h" //needed for trackbar class
#include "Time.h"
#include "WindowsFuncs.h"
#include <vector>
#include <string>
MenuWindow* pmenu_window{ nullptr };

MenuWindow::MenuWindow(std::shared_ptr<CrossHair> pCh, const std::string& title, const std::string& classname, HINSTANCE hInstance)
	:
	pCh(pCh),
	windowtitle(title),
	classname(classname),
	fontfam(L"Arial"),
	font(&fontfam, 14),
	brush_pinkgrad(Gdiplus::Point(0, 10),
		Gdiplus::Point(window_width, 10),
		Gdiplus::Color(255, 255, 255, 255),
		Gdiplus::Color(255, 255, 20, 147)),
	brush_gradred(Gdiplus::Point(0, 10),
		Gdiplus::Point(30, 10),
		Gdiplus::Color(255, 255, 20, 147),
		Gdiplus::Color(255, 255, 10, 50)),
	brush_red(Gdiplus::Color(255, 255, 10, 10)),
	pinkpen(Gdiplus::Color::HotPink, 3.0f)


{


	::pmenu_window = this;

	//init window class
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = classname.c_str();
	wc.hbrBackground = CreateSolidBrush(RGB(transparencycolor[0], transparencycolor[1], transparencycolor[2]));

	RegisterClass(&wc);

	hWnd = CreateWindowEx(WS_EX_CLIENTEDGE  | WS_EX_LAYERED,
		classname.c_str(),
		windowtitle.c_str(),
		
		WS_POPUP | WS_CLIPCHILDREN,
		100,
		100,
		window_width,
		window_height,
		nullptr,
		nullptr,
		hInstance,
		NULL);


	backbuffer = std::make_unique<Gdiplus::Bitmap>(window_width, window_height);
	offscreengraphics = std::make_unique<Gdiplus::Graphics>(backbuffer.get());

	itembackbuffer = std::make_unique<Gdiplus::Bitmap>(button_width, button_height);
	itemoffscreengraphics = std::make_unique<Gdiplus::Graphics>(itembackbuffer.get());

	ShowWindow(hWnd, SW_SHOW);
	//make the background of our window transparent
	MakeTransparent(hWnd, LWA_COLORKEY | LWA_ALPHA, RGB(transparencycolor[0], transparencycolor[1], transparencycolor[2]), 255);
}

//https://docs.microsoft.com/en-us/windows/win32/gdiplus/-gdiplus-creating-a-linear-gradient-use

void MenuWindow::CreateTrackbar(HWND & parenthwnd, HWND & hwnd, const std::array<int32_t,2> & minmax, const Gdiplus::Rect & rect)
{

	hwnd = CreateWindowEx(
		0,
		TRACKBAR_CLASS,
		" ",
		WS_CHILD |
		WS_VISIBLE |
		TBS_ENABLESELRANGE |
		TBS_TRANSPARENTBKGND,
		rect.X,
		rect.Y,
		rect.Width,
		rect.Height,
		parenthwnd,
		NULL,
		hInstance,
		NULL);

	SendMessage(hwnd, TBM_SETRANGE,
		(WPARAM)TRUE,                   //redraw flag 
		(LPARAM)MAKELONG(minmax[0], minmax[1]));  //min. & max. positions
		
}

void MenuWindow::CreateButton(HWND& parenthwnd, HWND& hwnd, const std::string& title, Gdiplus::Rect& rect)
{
	hwnd = CreateWindowEx(
		0,
		"BUTTON",
		title.c_str(),
		WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
		rect.X,
		rect.Y,
		rect.Width,
		rect.Height,
		parenthwnd,
		nullptr,
		hInstance,
		nullptr);

}

bool MenuWindow::OpenChooseColor(const HWND hwnd)
{
	//this array of colorrefs will store the custom color data the user chooses during the colormenu
	COLORREF acrCustClr[16];
	//fullopen to give the user the full color options
	//https://docs.microsoft.com/en-us/windows/win32/api/commdlg/ns-commdlg-choosecolora-r1
	CHOOSECOLORA ccstruct{};
	ccstruct.Flags = CC_FULLOPEN | CC_RGBINIT;
	ccstruct.lStructSize = sizeof(ccstruct);
	ccstruct.lpCustColors = acrCustClr;

	//if our button was clicked
	bool status;
	if (hwnd == pmenu_window->hwnd_button_crossrgb) {
		if (status = ChooseColor(reinterpret_cast<LPCHOOSECOLOR>(&ccstruct))) {

			auto rgb{ static_cast<COLORREF>(ccstruct.rgbResult) };
			pmenu_window->pCh->SetColor(Key::CrossColor, { GetRValue(rgb),GetGValue(rgb),GetBValue(rgb) },true);
		}
	}
	else if (hwnd == pmenu_window->hwnd_button_circlergb) {

		if (status = ChooseColor(reinterpret_cast<LPCHOOSECOLOR>(&ccstruct))) {

			auto rgb{ static_cast<COLORREF>(ccstruct.rgbResult) };
			pmenu_window->pCh->SetColor(Key::CircleColor, { GetRValue(rgb),GetGValue(rgb),GetBValue(rgb) },true);
		}
	}
	else if (hwnd == pmenu_window->hwnd_button_dotrgb) {

		if (status = ChooseColor(reinterpret_cast<LPCHOOSECOLOR>(&ccstruct))) {

			auto rgb{ static_cast<COLORREF>(ccstruct.rgbResult) };
			pmenu_window->pCh->SetColor(Key::DotColor, { GetRValue(rgb),GetGValue(rgb),GetBValue(rgb) },true);
		}
	}
	
	if (!status) {
		WindowsFuncs::OutPut("Hmmm...We couldn't get the color.");
		return true;
	}
	else
		return false;
}

void MenuWindow::OnPaint(HDC& hdc, HWND hwnd)
{
	using namespace Gdiplus;

	Gdiplus::Graphics graphics(hdc);
	 
	offscreengraphics->Clear(Gdiplus::Color(transparencycolor[0], transparencycolor[1], transparencycolor[2]));	

	for (auto& dot : dots) {
		dot.Clamp(5, 5, window_width - 5, window_height - 5);
		dot.UpdatePos();

		offscreengraphics->FillEllipse(&brush_gradred, { Gdiplus::Rect(dot.X(), dot.Y(), dot.Width(), dot.Height()) });
	}

	//redraw text on the parent window
	if (hwnd == hWnd) {
	
		offscreengraphics->DrawString(L"Cross Length", -1, &font, PointF(trackbar_rects[0].X, trackbar_rects[0].Y - 23), &brush_pinkgrad);
		offscreengraphics->DrawString(L"Circle Size", -1, &font, PointF(trackbar_rects[1].X, trackbar_rects[1].Y - 23), &brush_pinkgrad);
		offscreengraphics->DrawString(L"Transparency", -1, &font, PointF(trackbar_rects[2].X, trackbar_rects[2].Y - 23), &brush_pinkgrad);
		offscreengraphics->DrawString(L"DotSize", -1, &font, PointF(trackbar_rects[3].X, trackbar_rects[3].Y - 23), &brush_pinkgrad);
		offscreengraphics->DrawString(L"Thickness", -1, &font, PointF(trackbar_rects[4].X, trackbar_rects[4].Y - 23), &brush_pinkgrad);
		offscreengraphics->DrawString(L"Center Gap", -1, &font, PointF(trackbar_rects[5].X, trackbar_rects[5].Y - 23), &brush_pinkgrad);

		offscreengraphics->DrawString(L"Annie's CrossHair Program <3", -1, &font, PointF(220, 360), &brush_red);

		//finally present the image to the screen
		graphics.DrawImage(backbuffer.get(), 0, 0);
	
		
	}
}

void MenuWindow::OnDrawItem(DRAWITEMSTRUCT * drawitemstruct)
{
	using namespace Gdiplus;


	Gdiplus::Graphics graphics(drawitemstruct->hDC);
	itemoffscreengraphics->Clear(Gdiplus::Color(transparencycolor[0], transparencycolor[1], transparencycolor[2]));
	
	
	if (drawitemstruct->hwndItem == hwnd_button_crossrgb) {
		itemoffscreengraphics->DrawString(L"Change Cross Color", -1, &font, PointF(5,  8), &brush_gradred);
	}
	else if (drawitemstruct->hwndItem == hwnd_button_dotrgb) {
		itemoffscreengraphics->DrawString(L"Change Dot Color", -1, &font, PointF(5,  8), &brush_gradred);
	}
	else if (drawitemstruct->hwndItem == hwnd_button_circlergb) {
		itemoffscreengraphics->DrawString(L"Change Circle Color", -1, &font, PointF(5,  8), &brush_gradred);
	}
	else if (drawitemstruct->hwndItem == hwnd_button_repositionch) {
		itemoffscreengraphics->DrawString(L"Reposition CrossHair", -1, &font, PointF(5,  8), &brush_gradred);
	}

	itemoffscreengraphics->DrawRectangle(&pinkpen, 0, 0, button_width - 3, button_height - 3);

	graphics.DrawImage(itembackbuffer.get(), 0, 0);
}

MenuWindow::~MenuWindow()
{
}

LRESULT CALLBACK MenuWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static bool dragmainwindow{ false };
	static HBRUSH controlbrush{ nullptr };

	switch (msg)
	{
	case WM_HSCROLL: {
		//this message is sent when we scroll on our trackbars
		//get the trackbar position
		//lparam during this message is the handle to the trackbar
		auto hwnd{ reinterpret_cast<HWND>(lParam) };
		if (hwnd == pmenu_window->hwnd_trackbar_crosslen) {

			auto pos{ SendMessage(pmenu_window->hwnd_trackbar_crosslen,TBM_GETPOS,0,0) }; //send a message to our trackback window requesting the current trackbar position
			pmenu_window->pCh->Set(Key::CrossLength, pos, true); //write the key to our file and set it in our crosshair
		}
		else if (hwnd == pmenu_window->hwnd_trackbar_circlesz) {

			auto pos{ SendMessage(pmenu_window->hwnd_trackbar_circlesz,TBM_GETPOS,0,0) };	
			pmenu_window->pCh->Set(Key::CircleSize, pos, true);
		}
		else if (hwnd == pmenu_window->hwnd_trackbar_transparency) {

			auto pos{ SendMessage(pmenu_window->hwnd_trackbar_transparency,TBM_GETPOS,0,0) };
			pmenu_window->pCh->Set(Key::Transparency, pos,true);			
		}
		else if (hwnd == pmenu_window->hwnd_trackbar_dotsz) {

			auto pos{ SendMessage(pmenu_window->hwnd_trackbar_dotsz,TBM_GETPOS,0,0) };
			pmenu_window->pCh->Set(Key::DotSize, pos, true);
		}
		else if (hwnd == pmenu_window->hwnd_trackbar_thickness) {

			auto pos{ SendMessage(pmenu_window->hwnd_trackbar_thickness,TBM_GETPOS,0,0) };
			pmenu_window->pCh->Set(Key::Thickness, pos, true);
		}
		else if (hwnd == pmenu_window->hwnd_trackbar_centergap) {

			auto pos{ SendMessage(pmenu_window->hwnd_trackbar_centergap,TBM_GETPOS,0,0) };
			pmenu_window->pCh->Set(Key::CenterGap, pos, true);
		}
		break;
	}
	case WM_CREATE: {

		//load our cursor <3
		pmenu_window->hCursor = static_cast<HCURSOR>(LoadImage(nullptr, pmenu_window->cursorpath.c_str(), IMAGE_ICON, pmenu_window->iconsz[0], pmenu_window->iconsz[0], LR_LOADFROMFILE));
		if (pmenu_window->hCursor == NULL)
			WindowsFuncs::OutPut("Error Loading: " + pmenu_window->cursorpath + " GetLastError Returned Code: " + std::to_string(GetLastError()));


		//handle to window, min max trackbar values, dimensions of window/button
		pmenu_window->CreateTrackbar(hWnd,pmenu_window->hwnd_trackbar_crosslen, pmenu_window->trackbar_minmax[0], pmenu_window->trackbar_rects[0]);
		pmenu_window->CreateTrackbar(hWnd,pmenu_window->hwnd_trackbar_circlesz, pmenu_window->trackbar_minmax[1], pmenu_window->trackbar_rects[1]);
		pmenu_window->CreateTrackbar(hWnd,pmenu_window->hwnd_trackbar_transparency, pmenu_window->trackbar_minmax[2], pmenu_window->trackbar_rects[2]);
		pmenu_window->CreateTrackbar(hWnd,pmenu_window->hwnd_trackbar_dotsz, pmenu_window->trackbar_minmax[3], pmenu_window->trackbar_rects[3]);
		pmenu_window->CreateTrackbar(hWnd, pmenu_window->hwnd_trackbar_thickness, pmenu_window->trackbar_minmax[4], pmenu_window->trackbar_rects[4]);
		pmenu_window->CreateTrackbar(hWnd, pmenu_window->hwnd_trackbar_centergap, pmenu_window->trackbar_minmax[5], pmenu_window->trackbar_rects[5]);

		//handle to window, text to be displayed on button, dimensions of window/button
		pmenu_window->CreateButton(hWnd, pmenu_window->hwnd_button_crossrgb, "Change Cross Color", pmenu_window->button_rects[0]);
		pmenu_window->CreateButton(hWnd, pmenu_window->hwnd_button_dotrgb, "Change Dot Color", pmenu_window->button_rects[1]);
		pmenu_window->CreateButton(hWnd, pmenu_window->hwnd_button_circlergb, "Change Circle Color", pmenu_window->button_rects[2]);
		pmenu_window->CreateButton(hWnd, pmenu_window->hwnd_button_repositionch, "Reposition CrossHair", pmenu_window->button_rects[3]);

		break;
	}
	case WM_DRAWITEM: {
		//drawitem is passed when controls are being drawn.

		pmenu_window->OnDrawItem(reinterpret_cast<DRAWITEMSTRUCT*>(lParam));

		break;
	}
	case WM_CTLCOLORBTN: {

		//for owner drawn buttons this should return the background color for our button
		//return a null brush so it does not repaint our button causing flickering

		//system defined brushes retrieved with getstockobject do not need to be deleted
		return reinterpret_cast<LRESULT>(GetStockObject(NULL_BRUSH)); 
	}
	case WM_CTLCOLORSTATIC: {

		//passed when trackbars are being painted
		if (controlbrush == nullptr)
			controlbrush = CreateSolidBrush(RGB(pmenu_window->trackbarcolor[0], 
				pmenu_window->trackbarcolor[1],
				pmenu_window->trackbarcolor[2]));

		//return a brush which the is used to paint our trackbars background
		return reinterpret_cast<INT_PTR>(controlbrush);
	
	}
	case WM_ERASEBKGND: { //prevent the window from erasing the background causing flickering by returning 1
		return 1;
	}
	case WM_TIMER: {
	
		//tell windows to repaint our window when the timer is reached
		static RECT rect{ 0,0,pmenu_window->Width(),pmenu_window->Width() };
		InvalidateRect(hWnd, &rect, false);	
		
		break;
	}
	case WM_LBUTTONDOWN: {
		if (hWnd == pmenu_window->hWnd) {
			SetCapture(hWnd); // used to make sure after we left click that only our window is processing the mouse movements until we release left click
			dragmainwindow = true;
		}
		
		break;
	}
	case WM_LBUTTONUP: {
		if (hWnd == pmenu_window->hWnd) {
			ReleaseCapture(); //always release the mouse capture
			dragmainwindow = false;
		}
		break;
	}
	case WM_MOUSEMOVE: {	

	
		if (hWnd == pmenu_window->hWnd) {
			
			POINT mousecoordinates{ LOWORD(lParam), HIWORD(lParam) }; //get the current mouse coordinates in client space, then convert to screen space
			pmenu_window->mouseX = mousecoordinates.x;  //log the mouse coordinates
			pmenu_window->mouseY = mousecoordinates.y;

			static POINT prev_coordinates{ };			
			ClientToScreen(hWnd, &mousecoordinates);                 //convert the client space coordinates to screen space (desktop resolution)

			if (dragmainwindow) {

			RECT mainWindowRect{ 0 };
			GetWindowRect(pmenu_window->hWnd, &mainWindowRect); //get the windows current position and dimensions in screen-space
			auto movedby{ POINT{ mousecoordinates.x - prev_coordinates.x, mousecoordinates.y - prev_coordinates.y} }; //calculate how much we moved our mouse by since the last move
			MoveWindow(hWnd, mainWindowRect.left + movedby.x, mainWindowRect.top + movedby.y, pmenu_window->window_width, pmenu_window->window_height, true); //drag the window by the amount we are moving the mouse, without repositioning it to our cursor position

			}
			prev_coordinates = mousecoordinates; //store the current mouse coordinates for future calculations
		}
		break;
	}	
	case WM_SETCURSOR: {
		SetCursor(pmenu_window->hCursor);
		return true;
	}
	case WM_CLOSE: {

		DestroyWindow(hWnd);
		break;
	}
	case WM_QUIT: {
		DeleteObject(controlbrush);
		break;
	}
	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	}
	case WM_COMMAND: {

		auto hwnd{ reinterpret_cast<HWND>(lParam) };
		if (hwnd == pmenu_window->hwnd_button_circlergb ||
			hwnd == pmenu_window->hwnd_button_crossrgb ||
			hwnd == pmenu_window->hwnd_button_dotrgb)

			pmenu_window->OpenChooseColor(reinterpret_cast<HWND>(lParam));

		else if (hwnd == pmenu_window->hwnd_button_repositionch)
			pmenu_window->cond_signalreposition.notify_one();

		break;
    }
	case WM_PAINT: {
		
		PAINTSTRUCT ps;
		BeginPaint(pmenu_window->hWnd, &ps);
		pmenu_window->OnPaint(ps.hdc, hWnd);
		EndPaint(pmenu_window->hWnd, &ps);
		
		break;	
	}
   }

	//let our threads know that we changed a setting so it can update the crosshair

	   if (msg == WM_HSCROLL || msg == WM_COMMAND)
		   pmenu_window->cond_signalpaint.notify_one();


	return DefWindowProc(hWnd, msg, wParam, lParam);
}
