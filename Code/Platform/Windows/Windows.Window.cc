#include "PCH.hh"
#include "Platform/Windows/Windows.Window.hh"

#include "Platform/Windows/Windows.Utils.hh"

namespace ct::windows
{
	Window::Window(const std::string& title, int width, int height, int x, int y)
	{
		auto wideTitle {widenString(title)};
		NativeHandle = ::CreateWindow(TEXT(CT_APP_NAME), wideTitle.data(), WS_OVERLAPPEDWINDOW, x, y, width, height,
									  nullptr, nullptr, nullptr, nullptr);

		SwapChain = vulkan::SwapChain(NativeHandle);
	}

	Window::Window(Window&& other) noexcept
	{
		*this = std::move(other);
	}

	Window::~Window()
	{
		::DestroyWindow(NativeHandle);
	}

	Window& Window::operator=(Window&& other) noexcept
	{
		std::swap(NativeHandle, other.NativeHandle);
		return *this;
	}

	void Window::show()
	{
		::ShowWindow(NativeHandle, SW_SHOW);
	}
}
