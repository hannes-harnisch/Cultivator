#pragma once

#include "PCH.hh"

#include "App/WindowBase.hh"

namespace ct::windows
{
	class Window : public WindowBase
	{
	public:
		Window(std::string_view title, Rectangle size, int x, int y);

		Rectangle getViewport() const final override;
		void show() final override;
		void* handle() const final override;

	private:
		struct WindowDeleter
		{
			using pointer = HWND;
			void operator()(HWND handle)
			{
				::DestroyWindow(handle);
			}
		};
		std::unique_ptr<HWND, WindowDeleter> windowHandle;

		HWND makeAndGetWindowHandle(std::string_view title, Rectangle size, int x, int y);
	};
}
