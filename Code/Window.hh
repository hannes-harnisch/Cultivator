#pragma once

#include <string>

namespace ct
{
	class Window
	{
	public:
		Window() = default;
		Window(const std::string& title, int width, int height, int x, int y);

		void show();

	private:
		void* NativeHandle;
	};
}
