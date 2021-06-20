#pragma once
#include "CrossHair.h"
#include "WindowsFuncs.h"
#include <condition_variable>
#include <algorithm>
#include <vector>

class MenuWindow
{
public:
	class Dot {

	public:
		Dot(int32_t x, int32_t y) 
		: 
			x(x),
			y(y)
		{
		}
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
		int32_t width{ 25 };
		int32_t height{ 25 };
		int32_t velX{ 1 };
		int32_t velY{ 1 };
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


	std::vector<std::array<int32_t, 2>> trackbar_minmax{
		{ 0,170 }, //crosslenminmax
		{ 0,170 }, //circleszminmax
		{ 0,255 }, //transpminmax
		{ 0,170 }, //dotszminmax
		{ 1,10 },  //thicknessminmax
		{ -10,170 }//centergapminmax
	};


	//x,y,width,height
	std::vector<Gdiplus::Rect> trackbar_rects{
	   Gdiplus::Rect( 10,30,150,30 ),  //crosslen
	   Gdiplus::Rect( 10,90,150,30 ),  //circlesz
	   Gdiplus::Rect( 10,150,160,30 ), //transparency
	   Gdiplus::Rect( 10,210,150,30 ), //dotsz
	   Gdiplus::Rect( 10,270,80,30 ),  //thickness
	   Gdiplus::Rect( 10,330,150,30 )  //centergap
	};
	HWND hwnd_trackbar_thickness{ nullptr };
	HWND hwnd_trackbar_circlesz{ nullptr };
	HWND hwnd_trackbar_crosslen{ nullptr };
	HWND hwnd_trackbar_transparency{ nullptr };
	HWND hwnd_trackbar_dotsz{ nullptr };
	HWND hwnd_trackbar_centergap{ nullptr };

	/****************buttons******************************/

	int32_t button_width{ 200 },  button_height{ 40 };

	std::vector<Gdiplus::Rect> button_rects{
	   Gdiplus::Rect(280,10,button_width,button_height),  //crosschangecolor
	   Gdiplus::Rect(280,70,button_width,button_height),  //dotchangecolor
	   Gdiplus::Rect(280,130,button_width,button_height), //circlechangecolor
	   Gdiplus::Rect(280,190,button_width,button_height), //resposition
	};

	HWND hwnd_button_crossrgb{ nullptr };
	HWND hwnd_button_dotrgb{ nullptr };
	HWND hwnd_button_circlergb{ nullptr };
	HWND hwnd_button_repositionch{ nullptr };

	/***********************************************/

	std::shared_ptr<CrossHair> pCh;


	/***********************************************/

	std::vector<Dot> dots{
		Dot(100, 200),
		Dot(200, 400),
		Dot(0, 148),
		Dot(267, 400),
		Dot(500, 400),
	};

	//drawing properties
	Gdiplus::FontFamily fontfam;
	Gdiplus::Font font;
	Gdiplus::LinearGradientBrush brush_pinkgrad;
	Gdiplus::LinearGradientBrush brush_gradred;
	Gdiplus::SolidBrush brush_red;
	Gdiplus::Pen pinkpen;


	std::unique_ptr<Gdiplus::Bitmap> backbuffer;
	std::unique_ptr<Gdiplus::Graphics> offscreengraphics;
	std::unique_ptr<Gdiplus::Bitmap> itembackbuffer;
	std::unique_ptr<Gdiplus::Graphics> itemoffscreengraphics;

public:
	std::condition_variable cond_signalpaint;
	std::condition_variable cond_signalreposition; //used to signal thread to allow click-drag of crosshair window
};
