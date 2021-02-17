#include "PCH.hh"
#include "Vendor/Windows/Windows.Window.hh"

#include "Vendor/Windows/Windows.Utils.hh"

namespace ct::windows
{
	Window::Window(const std::string& title, Rectangle size, int x, int y) :
		WindowBase {makeNativeHandle(title, size, x, y), getViewport()}
	{}

	Window::~Window()
	{
		::DestroyWindow(static_cast<HWND>(NativeHandle));
	}

	Rectangle Window::getViewport()
	{
		RECT rect;
		::GetClientRect(static_cast<HWND>(NativeHandle), &rect);
		return {uint32_t(rect.right), uint32_t(rect.bottom)};
	}

	void Window::show()
	{
		::ShowWindow(static_cast<HWND>(NativeHandle), SW_SHOW);
	}

	HWND Window::makeNativeHandle(const std::string& title, Rectangle size, int x, int y)
	{
		auto wideTitle {widenString(title)};
		return ::CreateWindow(TEXT(CT_APP_NAME), wideTitle.data(), WS_OVERLAPPEDWINDOW, x, y, size.Width, size.Height,
							  nullptr, nullptr, nullptr, nullptr);
	}
}
