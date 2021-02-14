#pragma once

#include "PCH.hh"
#include "WindowBase.hh"

namespace ct::windows
{
	class Window : public WindowBase
	{
	public:
		Window() = default;
		Window(const std::string& title, int width, int height, int x, int y);
		Window(Window&& other) noexcept;
		~Window();
		Window& operator=(Window&& other) noexcept;

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		void show() override;

	private:
		HWND NativeHandle;
	};
}
