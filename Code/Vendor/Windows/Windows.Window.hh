#pragma once

#include "PCH.hh"

#include "App/WindowBase.hh"

namespace ct::windows
{
	class Window final : public WindowBase
	{
	public:
		Window(const std::string& title, Rectangle size, int x, int y);
		~Window();
		Window(Window&& other) noexcept;
		Window& operator=(Window&& other) noexcept;

		Rectangle getViewport() override;
		void show() override;

	private:
		HWND WindowHandle;

		HWND createWindowHandle(const std::string& title, Rectangle size, int x, int y);
	};
}
