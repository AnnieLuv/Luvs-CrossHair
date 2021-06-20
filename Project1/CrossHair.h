#pragma once
#include <stdint.h>
#include <array>
#include <memory>
#include <fstream>
#include <sstream>
#include <exception>

enum class Key {
	Begin = 0,
	CircleColor,
	CrossColor,
	DotColor,
	Thickness,
	DotSize,
	CircleSize,
	CrossLength,
	Transparency,
	CenterGap,
	End
};
class CrossHair {
  
public:
	void Set(Key key, const int32_t val, bool replaceentry);
	int32_t Get(Key key);
	const std::array<uint8_t, 3> GetColor(Key key) const;
	void SetColor(Key key, const std::array<uint8_t, 3>& color, bool replaceentry);
	bool ReplaceEntry(Key key, int32_t valuetowrite, const std::array<uint8_t, 3>& color, bool iscolor);
	bool ReadEntries();

private:
	int32_t thickness{ 2 };
	std::array<uint8_t,3> crossColor{ 0,255,0 };
	std::array<uint8_t, 3> circleColor{ 0,255,0 };
	std::array<uint8_t, 3> dotColor{ 0,255,0 };
	int32_t crossLength{ 25 };
	int32_t circleSize{ 28 };
	int32_t transparency{ 255 };
	int32_t dotSize{ 5 };
	int32_t centergap{ -10 };
	std::string configfilename{ "DoNotTouchThisConfig.txt" };
	
public:
		//std::shared_ptr<Gdiplus::Bitmap> pBmp;
};
