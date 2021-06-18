#pragma once
#include "CrossHair.h"
#include "WindowsFuncs.h"
#include <condition_variable>
#include <algorithm>

class MenuWindow
{
public:
	class Dot {

	public:
		void Clamp(int32_t X, int32_t Y, int32_t Width, int32_t Height) {


			auto collidingx = std::clamp(x, X, Width);
			auto collidingy = std::clamp(y, Y, Height);

			//set the positions to clamp the dot inside the specified region
			x = collidingx;
			y = collidingy;

			//then reverse the move direction
			if (collidingx == Width || collidingx == X)
				velX *= -1;
			if (collidingy == Height || collidingy == Y)
				velY *= -1;
		}
		void UpdatePos() {
			x += velX;
			y += velY;
		}
		int32_t X()const {
			return x;
		}
		int32_t Y()const {
			return y;
		}
		int32_t Height()const {
			return height;
		}
		int32_t Width()const {
			return width;
		}
	private:
		int32_t x{ 100 };
		int32_t y{ 0 };
		int32_t width{ 15 };
		int32_t height{ 15 };
		int32_t velX{ 5 };
		int32_t velY{ 5 };
	};

	MenuWindow(std::shared_ptr<CrossHair> pCh, const std::string& title, const std::string& classname, HINSTANCE hInstance);
	const int Width()const {
		return window_width;
	}
	const int Height()const {
		return window_height;
	}
	HWND GetHandle() {
		return hWnd;
	}
	void CreateTrackbar(HWND & _hwnd, HWND & hwnd, const std::array<int32_t, 2>& minmax, const Gdiplus::Rect& rect);
	void CreateButton(HWND& _hwnd, HWND& hwnd, const std::string& title, Gdiplus::Rect& rect);
	bool OpenChooseColor(const HWND hwnd);
	void OnPaint(HDC& hdc, HWND hwnd);
	void OnDrawItem(DRAWITEMSTRUCT * drawitemstruct);
	~MenuWindow();
private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool MakeTransparent(HWND hWnd, int32_t flags, COLORREF colortomakeinvis, uint8_t transparencystrength)
	{
		//for layered windows do not make the colortomakeinvis r g b the same ex: "0,0,0" because the window wont process mouse messages
		//colortomakeinvis is in R,G,B
		return SetLayeredWindowAttributes(hWnd, colortomakeinvis, transparencystrength, flags);
	}
private:
	/**************window properties****************/
	std::array<int32_t, 2> iconsz{ 30,40 };
	std::string cursorpath{ "hearticon.ico" };
	HCURSOR hCursor{ nullptr };
	HWND hWnd{ nullptr };
	std::string classname;
	std::string windowtitle;
	WNDCLASS wc{};
	HINSTANCE hInstance{ nullptr };
	int32_t window_width{500};
	int32_t window_height{400};
	std::array<int32_t, 3> transparencycolor{ 12, 12, 10 }; //for some reason changing the transparency color causes the window to glitch...leave it like this
	/**************mouse***************************/

	int32_t mouseX{ 0 };
	int32_t mouseY{ 0 };
	

	/****************trackbar controls**********************/
	std::array<int32_t, 3> trackbarcolor{ 255, 192, 201 };
	//x,y,width,height

	std::array<int32_t, 2> trackbar_crosslen_minmax{ 0,170 };
	std::array<int32_t, 2> trackbar_circlesz_minmax{ 0,170 };
	std::array<int32_t, 2> trackbar_transparency_minmax{ 0,255 };
	std::array<int32_t, 2> trackbar_dotsz_minmax{ 0,170 };
	std::array<int32_t, 2> trackbar_thickness_minmax{ 1,10 };
	std::array<int32_t, 2> trackbar_centergap_minmax{ -10,170 };
	Gdiplus::Rect trackbar_crosslen{ 10,30,150,30 };
	Gdiplus::Rect trackbar_circlesz{ 10,90,150,30 };
	Gdiplus::Rect trackbar_transparency{ 10,150,160,30 };
	Gdiplus::Rect trackbar_dotsz{ 10,210,150,30 };
	Gdiplus::Rect trackbar_thickness{ 10,270,80,30 };
	Gdiplus::Rect trackbar_centergap{ 10,330,150,30 };
	HWND hwnd_trackbar_thickness{ nullptr };
	HWND hwnd_trackbar_circlesz{ nullptr };
	HWND hwnd_trackbar_crosslen{ nullptr };
	HWND hwnd_trackbar_transparency{ nullptr };
	HWND hwnd_trackbar_dotsz{ nullptr };
	HWND hwnd_trackbar_centergap{ nullptr };

	/****************buttons******************************/

	Gdiplus::Rect button_crossrgb{ 280,10,200,40 }; 	//x,y,width,height
	Gdiplus::Rect button_dotrgb{ 280,70,200,40 };
	Gdiplus::Rect button_circlergb{ 280,130,200,40 };
	Gdiplus::Rect button_repositionch{ 280,190,200,40 };
	HWND hwnd_button_crossrgb{ nullptr };
	HWND hwnd_button_dotrgb{ nullptr };
	HWND hwnd_button_circlergb{ nullptr };
	HWND hwnd_button_repositionch{ nullptr };
	/***********************************************/

	std::shared_ptr<CrossHair> pCh;
	std::shared_ptr<Gdiplus::Bitmap> ch_pBmp;

	/***********************************************/
public:
	std::condition_variable cond_signalpaint;
	std::condition_variable cond_signalreposition; //used to signal thread to allow click-drag of crosshair window
};
