#pragma once

#include "PCH.hh"

#include "App/WindowBase.hh"

namespace ct::windows
{
	class Window final : public WindowBase
	{
	public:
		Window(std::string const& title, Rectangle size, int x, int y);

		Rectangle getViewport() override;
		void show() override;

	private:
		struct WindowDeleter
		{
			using pointer = HWND;
			void operator()(HWND handle)
			{
				::DestroyWindow(handle);
			}
		};
		std::unique_ptr<HWND, WindowDeleter> WindowHandle;

		HWND makeAndGetWindowHandle(std::string const& title, Rectangle size, int x, int y);
	};
}
