#include "Window.hh"

#include "Windows/Windows.API.hh"
#include "Windows/Windows.Utils.hh"

namespace ct
{
	Window::Window(const std::string& title, int width, int height, int x, int y)
	{
		auto wideTitle = widenString(title);
		NativeHandle   = ::CreateWindow(TEXT(CT_APP_NAME), wideTitle.data(), WS_OVERLAPPEDWINDOW, x, y, width, height,
										nullptr, nullptr, nullptr, nullptr);
	}

	void Window::show()
	{
		HWND handle = static_cast<HWND>(NativeHandle);
		::ShowWindow(handle, SW_SHOW);
	}
}
