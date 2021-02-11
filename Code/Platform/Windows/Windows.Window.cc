#include "Window.hh"

#include "Platform/Windows/Windows.API.hh"
#include "Platform/Windows/Windows.Utils.hh"

namespace ct
{
	Window::Window(const std::string& title, int width, int height, int x, int y)
	{
		auto wideTitle {widenString(title)};
		NativeHandle = ::CreateWindow(TEXT(CT_APP_NAME), wideTitle.data(), WS_OVERLAPPEDWINDOW, x, y, width, height,
									  nullptr, nullptr, nullptr, nullptr);
	}

	Window::Window(Window&& other) noexcept
	{
		*this = std::move(other);
	}

	Window::~Window()
	{
		HWND handle {static_cast<HWND>(NativeHandle)};
		::DestroyWindow(handle);
	}

	Window& Window::operator=(Window&& other) noexcept
	{
		std::swap(NativeHandle, other.NativeHandle);
		return *this;
	}

	void Window::show()
	{
		HWND handle {static_cast<HWND>(NativeHandle)};
		::ShowWindow(handle, SW_SHOW);
	}
}
