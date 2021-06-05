#include "PCH.hh"

#include "Vendor/Windows/Windows.AppContext.hh"
#include "Vendor/Windows/Windows.Utils.hh"
#include "Vendor/Windows/Windows.Window.hh"

namespace ct::windows
{
	Window::Window(std::string_view const title, Rectangle const size, int const x, int const y) :
		windowHandle(makeAndGetWindowHandle(title, size, x, y))
	{}

	Rectangle Window::getViewport() const
	{
		RECT rect;
		::GetClientRect(windowHandle.get(), &rect);
		return {uint32_t(rect.right), uint32_t(rect.bottom)};
	}

	void Window::show()
	{
		::ShowWindow(windowHandle.get(), SW_SHOW);
	}

	void* Window::handle() const
	{
		return windowHandle.get();
	}

	HWND Window::makeAndGetWindowHandle(std::string_view const title, Rectangle const size, int const x, int const y)
	{
		auto wideTitle = widenString(title);
		return ::CreateWindow(AppContext::WindowClassName, wideTitle.data(), WS_OVERLAPPEDWINDOW, x, y, size.width, size.height,
							  nullptr, nullptr, AppContext::nativeInstanceHandle(), nullptr);
	}
}
