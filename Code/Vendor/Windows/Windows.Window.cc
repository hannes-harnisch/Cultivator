#include "PCH.hh"

#include "Vendor/Windows/Windows.AppContext.hh"
#include "Vendor/Windows/Windows.Utils.hh"
#include "Vendor/Windows/Windows.Window.hh"

namespace ct::windows
{
	Window::Window(std::string const& title, Rectangle size, int x, int y) :
		WindowBase(makeAndGetWindowHandle(title, size, x, y), getViewport())
	{}

	Rectangle Window::getViewport()
	{
		RECT rect;
		::GetClientRect(WindowHandle.get(), &rect);
		return {uint32_t(rect.right), uint32_t(rect.bottom)};
	}

	void Window::show()
	{
		::ShowWindow(WindowHandle.get(), SW_SHOW);
	}

	HWND Window::makeAndGetWindowHandle(std::string const& title, Rectangle size, int x, int y)
	{
		auto wideTitle = widenString(title);
		HWND handle	   = ::CreateWindow(AppContext::WindowClassName, wideTitle.data(), WS_OVERLAPPEDWINDOW, x, y, size.Width,
										size.Height, nullptr, nullptr, AppContext::nativeInstanceHandle(), nullptr);
		WindowHandle.reset(handle);
		return WindowHandle.get();
	}
}
