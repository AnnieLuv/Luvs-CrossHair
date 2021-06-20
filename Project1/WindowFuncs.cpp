#include "WindowsFuncs.h"
#include <string>
#include "Windows.h"
#include <gdiplus.h>


bool WindowsFuncs::InitGDIPlus(ULONG_PTR gdiplusToken){

	//gdi+ uses RAII and is object oriented so we don't need to manually delete everything like in win32GDI functions!!
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;

	//initialize GDI+
	//token is used for shutting gdi down when done
	auto status{ Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr) };
	if (status != Gdiplus::Status::Ok) {
		OutPut(getGdiplusStatusMessage(status));
		return false;
	}
	else
		return true;
}

std::string WindowsFuncs::GetLastErrorAsString() {

	auto errcode{ GetLastError() };

	std::string msgbuffer(200, ' ');;

	//we dont want to use the localfree functions and stuff so we will pass in our own std::string and not let the function allocate memory ffor us
	auto size{ FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		errcode,
		LANG_USER_DEFAULT,
		reinterpret_cast<LPSTR>(msgbuffer.data()),  //passing it the pointer's address (pointer-to-a-pointer), but casting it to a regular pointer... Win32 weirdness.
		msgbuffer.length(),
		NULL) };

	return msgbuffer;

}

bool WindowsFuncs::IsKeyDown(int32_t key)
{
	return (GetAsyncKeyState(key) & 0b1000000000000000); //check if most significant bit is set
}

std::string WindowsFuncs::getGdiplusStatusMessage(Gdiplus::Status status)
{
	using  namespace Gdiplus;

	std::string msg;
	switch (status)
	{
	case Status::Ok:
		msg = "Ok: Indicates that the method call was successful.";
		break;
	case Status::GenericError:
		msg = "GenericError: Indicates that there was an error on the method call, which is identified as something other than those defined by the other elements of this enumeration.";
		break;
	case Status::InvalidParameter:
		msg = "InvalidParameter: Indicates that one of the arguments passed to the method was not valid.";
		break;
	case Status::OutOfMemory:
		msg = "OutOfMemory: Indicates that the operating system is out of memory and could not allocate memory to process the method call.";
		break;
	case Status::ObjectBusy:
		msg = "ObjectBusy: Indicates that one of the arguments specified in the API call is already in use in another thread.";
		break;
	case Status::InsufficientBuffer:
		msg = "InsufficientBuffer: Indicates that a buffer specified as an argument in the API call is not large enough to hold the data to be received.";
		break;
	case Status::NotImplemented:
		msg = "NotImplemented: Indicates that the method is not implemented.";
		break;
	case Status::Win32Error:
		msg = "Win32Error: Indicates that the method generated a Win32 error.";
		break;
	case Status::WrongState:
		msg = "WrongState: Indicates that the object is in an invalid state to satisfy the API call. For example, calling Pen::GetColor from a pen that is not a single, solid color results in a WrongState status.";
		break;
	case Status::Aborted:
		msg = "Aborted: Indicates that the method was aborted.";
		break;
	case Status::FileNotFound:
		msg = "FileNotFound: Indicates that the specified image file or metafile cannot be found.";
		break;
	case Status::ValueOverflow:
		msg = "ValueOverflow: Indicates that the method performed an arithmetic operation that produced a numeric overflow.";
		break;
	case Status::AccessDenied:
		msg = "AccessDenied: Indicates that a write operation is not allowed on the specified file.";
		break;
	case Status::UnknownImageFormat:
		msg = "UnknownImageFormat: Indicates that the specified image file format is not known.";
		break;
	case Status::FontFamilyNotFound:
		msg = "FontFamilyNotFound: Indicates that the specified font family cannot be found. Either the font family name is incorrect or the font family is not installed.";
		break;
	case Status::FontStyleNotFound:
		msg = "FontStyleNotFound: Indicates that the specified style is not available for the specified font family.";
		break;
	case Status::NotTrueTypeFont:
		msg = "NotTrueTypeFont: Indicates that the font retrieved from an HDC or LOGFONT is not a TrueType font and cannot be used with GDI+.";
		break;
	case Status::UnsupportedGdiplusVersion:
		msg = "UnsupportedGdiplusVersion: Indicates that the version of GDI+ that is installed on the system is incompatible with the version with which the application was compiled.";
		break;
	case Status::GdiplusNotInitialized:
		msg = "GdiplusNotInitialized: Indicates that the GDI+ API is not in an initialized state. To function, all GDI+ objects require that GDI+ be in an initialized state. Initialize GDI+ by calling GdiplusStartup.";
		break;
	case Status::PropertyNotFound:
		msg = "PropertyNotFound: Indicates that the specified property does not exist in the image.";
		break;
	case Status::PropertyNotSupported:
		msg = "PropertyNotSupported: Indicates that the specified property is not supported by the format of the image and, therefore, cannot be set.";
		break;
	default:
		msg = "Invalid status: Indicates an unknown status was returned.";
		break;
	}
	return msg;
}

void WindowsFuncs::OutPut(const std::string& err)
{
	MessageBox(0, err.c_str(), "~ Message ~", MB_OK);
}

bool WindowsFuncs::OutPutYesNo(const std::string& err)
{

	if (MessageBox(0, err.c_str(), "~ Message ~", MB_YESNO) == IDYES)
		return true;
	else
		return false;

}

void WindowsFuncs::GdiToHighQuality(Gdiplus::Graphics* graphics)
{
	graphics->SetCompositingMode(Gdiplus::CompositingModeSourceOver);
	graphics->SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
	graphics->SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
	graphics->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	graphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);
}

void WindowsFuncs::GdiToLowQuality(Gdiplus::Graphics* graphics)
{
	graphics->SetCompositingMode(Gdiplus::CompositingModeSourceOver);
	graphics->SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
	graphics->SetPixelOffsetMode(Gdiplus::PixelOffsetModeNone);
	graphics->SetSmoothingMode(Gdiplus::SmoothingModeNone);
	graphics->SetInterpolationMode(Gdiplus::InterpolationModeLowQuality);

}
