#include "PCH.hh"
#include "Vendor/Windows/Windows.Window.hh"

#include "Vendor/Windows/Windows.AppContext.hh"
#include "Vendor/Windows/Windows.Utils.hh"

namespace ct::windows
{
	Window::Window(const std::string& title, Rectangle size, int x, int y) :
		WindowBase {WindowHandle = createWindowHandle(title, size, x, y), getViewport()}
	{}

	Window::~Window()
	{
		::DestroyWindow(WindowHandle);
	}

	Window::Window(Window&& other) noexcept :
		WindowBase {std::move(other)}, WindowHandle {std::exchange(other.WindowHandle, nullptr)}
	{}

	Window& Window::operator=(Window&& other) noexcept
	{
		WindowBase::operator=(std::move(other));
		std::swap(WindowHandle, other.WindowHandle);
		return *this;
	}

	Rectangle Window::getViewport()
	{
		RECT rect;
		::GetClientRect(WindowHandle, &rect);
		return {uint32_t(rect.right), uint32_t(rect.bottom)};
	}

	void Window::show()
	{
		::ShowWindow(WindowHandle, SW_SHOW);
	}

	HWND Window::createWindowHandle(const std::string& title, Rectangle size, int x, int y)
	{
		auto wideTitle {widenString(title)};
		return ::CreateWindow(AppContext::WindowClassName, wideTitle.data(), WS_OVERLAPPEDWINDOW, x, y, size.Width,
							  size.Height, nullptr, nullptr, AppContext::nativeInstanceHandle(), nullptr);
	}
}
