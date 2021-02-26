#pragma once

#include "PCH.hh"
#include "Utils/Rectangle.hh"

namespace ct::vulkan
{
	class Texture final
	{
	public:
		Texture(Rectangle size);
		~Texture();
		Texture(Texture&& other) noexcept;
		Texture& operator=(Texture&& other) noexcept;

	private:
		vk::Image Image;
		vk::DeviceMemory Memory;

		vk::Image createImage(Rectangle size);
		vk::DeviceMemory allocateMemory();
	};
}
