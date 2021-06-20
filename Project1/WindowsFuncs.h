#pragma once
#include "Windows.h"
#include <gdiplus.h>
#include <string>

namespace WindowsFuncs {

	bool InitGDIPlus(ULONG_PTR gdiplusToken);
	static std::string GetLastErrorAsString();
	bool IsKeyDown(int32_t key);
	std::string getGdiplusStatusMessage(Gdiplus::Status status);
	void OutPut(const std::string& err);
	bool OutPutYesNo(const std::string& err);
	void GdiToHighQuality(Gdiplus::Graphics * graphics);
	void GdiToLowQuality(Gdiplus::Graphics * graphics);
	
}