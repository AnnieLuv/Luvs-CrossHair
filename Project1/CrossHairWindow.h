#pragma once
#include <string>
#include "Time.h"
#include "CrossHair.h"
#include "WindowsFuncs.h"


class CrossHairWindow
{
public:

	CrossHairWindow(std::shared_ptr<CrossHair> pCh,int32_t width, int32_t height, const std::string& title, const std::string& classname,  HINSTANCE hInstance);
	int Width()const{
		return width;
	}
	int Height()const {
		return height;
	}
	HWND GetHandle() {
		return hWnd;
	}
	void OnPaint(HDC& hdc);
	~CrossHairWindow();
private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool MakeTransparent(HWND hWnd, int32_t flags, COLORREF colortomakeinvis, uint8_t transparencystrength)
	{
		//for layered windows do not make the colortomakeinvis r g b the same ex: "0,0,0" because the window wont process mouse messages
		//colortomakeinvis is in R,G,B
		return SetLayeredWindowAttributes(hWnd, colortomakeinvis, transparencystrength, flags);
	}
private:
	HWND hWnd{ nullptr };
	std::string classname;
	std::string windowtitle;
	WNDCLASS wc{};
	HINSTANCE hInstance;
	int32_t width{ 300 };
	int32_t height{ 300 };
    /**********************************************/
	std::shared_ptr<CrossHair> pCh;
	std::array<uint8_t, 3> transpcolor{ 12,10,10 };

	/*********************************************/
	Gdiplus::Pen ch_cross_pen;
	Gdiplus::SolidBrush ch_cross_brush;
	Gdiplus::Pen ch_circle_pen;
	Gdiplus::SolidBrush	ch_circle_brush;
	Gdiplus::SolidBrush	ch_dot_brush;

	std::unique_ptr<Gdiplus::Graphics> graphics;
	std::unique_ptr<Gdiplus::Graphics> offscreengraphics;
	std::unique_ptr<Gdiplus::Bitmap> backbuffer;
	/***********************************************/
};
