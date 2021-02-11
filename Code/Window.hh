#pragma once

#include "Platform/Vulkan/Vulkan.Adapter.hh"
#include <string>

namespace ct
{
	class Window
	{
	public:
		Window() = default;
		Window(const std::string& title, int width, int height, int x, int y);
		Window(Window&& other) noexcept;
		~Window();
		Window& operator=(Window&& other) noexcept;

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		void show();

	private:
		void* NativeHandle;

		vulkan::Adapter GraphicsAdapter;
	};
}
