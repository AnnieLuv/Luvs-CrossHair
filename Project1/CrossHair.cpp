#include "CrossHair.h"
#include "WindowsFuncs.h"
#include <functional>

void CrossHair::Set(Key key, const int32_t val, bool replaceentry) { //pass any non color property here

	switch (key) {
	case Key::Thickness: {
		thickness = val;
		break;
	}
	case Key::CrossLength:
		crossLength = val;
		break;
	case Key::CircleSize:
		circleSize = val;
		break;
	case Key::Transparency:
		transparency = val;
		break;
	case Key::DotSize:
		dotSize = val;
		break;
	case Key::CenterGap:
		centergap = val;
		break;
	default:
		WindowsFuncs::OutPut("Error: No such key exists.\nFile: CrossHair.cpp\nFunction: Set");
		return;
	}

	if (replaceentry)
		ReplaceEntry(key, val, {},false); //replace the key in the file
}
int32_t CrossHair::Get(Key key) { //pass any non color property here

	switch (key) {
	case Key::Thickness:
		return thickness;
	case Key::CrossLength:
		return crossLength;
	case Key::CircleSize:
		return circleSize;
	case Key::Transparency:
		return transparency;
	case Key::DotSize:
		return dotSize;
	case Key::CenterGap:
		return centergap;
	default:
		WindowsFuncs::OutPut("Error: No such key exists.\nFile: CrossHair.cpp\nFunction: Get");
		return{};
	}
}
const std::array<uint8_t, 3> CrossHair::GetColor(Key key) const {
	switch (key) {
	case Key::CircleColor:
		return circleColor;
	case Key::CrossColor:
		return crossColor;
	case Key::DotColor:
		return dotColor;
	default:
		WindowsFuncs::OutPut("Error: No such key exists.\nFile: CrossHair.cpp\nFunction: GetColor");
		return {};
	}
}
void CrossHair::SetColor(Key key, const std::array<uint8_t, 3>& color, bool replaceentry = true) {

	switch (key) {
	case Key::CircleColor:
		circleColor = color;
		break;
	case Key::CrossColor:
		crossColor = color;
		break;
	case Key::DotColor:
		dotColor = color;
		break;
	default:
		WindowsFuncs::OutPut("Error: No such key exists.\nFile: CrossHair.cpp\nFunction: SetColor");
		return;
	}

	if (replaceentry)
		ReplaceEntry(key, 0, color, true);

}

bool CrossHair::ReplaceEntry(Key key, int32_t valuetowrite, const std::array<uint8_t, 3>& color, bool iscolor)
 {

	std::ifstream in(configfilename);

	if (!in.is_open()) {
		WindowsFuncs::OutPut("We couldn't find " + configfilename);
		return false;
	}

	std::string line{};
	std::stringstream buffer{};
	size_t curr_pos{ static_cast<size_t>(Key::CircleColor) };


	while (std::getline(in, line)) {

		//ignore comments
		if (line.find("//") != std::string::npos) {
			buffer << line << "\n";
			continue;
		}
		if (curr_pos == static_cast<size_t>(key)) {
			if (iscolor)
				buffer << static_cast<int32_t>(color[0]) << " " << static_cast<int32_t>(color[1]) << " " << static_cast<int32_t>(color[2]) << "\n";
			else
				buffer << valuetowrite << "\n";
		}
		else
			buffer << line << "\n";

		//increment curr_pos to let us know which entry we are reading. (which key) 
		++curr_pos;
	}

	in.close();
	std::ofstream out(configfilename, std::ios::trunc); //remove all text from the file so we can replace it with the new buffer
	out << buffer.str();
	out.close();
}
bool CrossHair::ReadEntries() {


	std::ifstream in(configfilename);

	if (!in.is_open()) {
		WindowsFuncs::OutPut("We couldn't find " + configfilename);
		return false;
	}

	std::string line{};
	std::stringstream buffer{};
	size_t curr_pos{ static_cast<size_t>(Key::CircleColor) };

	//use these lambdas for properties that have the same method of data extraction
	auto proc{ [&](const std::string& line, Key key) {

			try {
				auto value{ std::stoi(line) };
				Set(key,value, false);
			}
			catch (std::exception& e) {
				WindowsFuncs::OutPut("FileError: " + std::string(e.what()) + "\n" + configfilename + "\nKey Number : " + std::to_string(static_cast<int>(key)) + "\nLine: " + line);
			};
 }
	};
	auto proc_color{ [&](const std::string& line, Key key) {

			try {
				std::stringstream rgb(line);
				std::string value;

				rgb >> value;
				auto r{ static_cast<uint8_t>(std::stoi(value)) };
				rgb >> value;
				auto g{ static_cast<uint8_t>(std::stoi(value)) };
				rgb >> value;
				auto b{ static_cast<uint8_t>(std::stoi(value)) };

				SetColor(key, { r,g,b }, false);
			}
			catch (std::exception& e) {
				WindowsFuncs::OutPut("FileError: " + std::string(e.what()) + "\n" + configfilename + "\nKey Number : " + std::to_string(static_cast<int>(key)) + "\nLine: " + line);
			};
		}
	};

	while (std::getline(in, line)) {

		//ignore comments
		if (line.find("//") != std::string::npos)
			continue;

		if (curr_pos == static_cast<size_t>(Key::CircleColor) ||
			curr_pos == static_cast<size_t>(Key::CrossColor) ||
			curr_pos == static_cast<size_t>(Key::DotColor)) {

			proc_color(line, static_cast<Key>(curr_pos));
		}
		else
			proc(line, static_cast<Key>(curr_pos));

		//increment curr_pos to let us know which entry we are reading. (which key) 
		//because our Keys are from 1 through N
		++curr_pos;
	}

	in.close();

}