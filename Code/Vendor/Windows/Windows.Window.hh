#pragma once

#include "PCH.hh"
#include "WindowBase.hh"

namespace ct::windows
{
	class Window final : public WindowBase
	{
	public:
		Window(const std::string& title, Rectangle size, int x, int y);
		~Window();

		Rectangle getViewport() final override;
		void show() final override;

	private:
		HWND makeNativeHandle(const std::string& title, Rectangle size, int x, int y);
	};
}
