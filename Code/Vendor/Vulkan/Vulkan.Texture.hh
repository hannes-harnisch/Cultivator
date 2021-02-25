#pragma once

#include "PCH.hh"

namespace ct::vulkan
{
	class Texture final
	{
	public:
		Texture();
		~Texture();
		Texture(Texture&& other) noexcept;
		Texture& operator=(Texture&& other) noexcept;

	private:
		vk::Image Image;
		vk::DeviceMemory Memory;
	};
}
