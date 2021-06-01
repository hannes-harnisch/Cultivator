#pragma once

#include "PCH.hh"

#include "Utils/Rectangle.hh"
#include "Vendor/Vulkan/Vulkan.Unique.hh"

namespace ct::vulkan
{
	class Texture final
	{
	public:
		Texture(Rectangle size);

	private:
		DeviceUnique<vk::Image, &vk::Device::destroyImage> Image;
		DeviceUnique<vk::DeviceMemory, &vk::Device::freeMemory> Memory;
		DeviceUnique<vk::ImageView, &vk::Device::destroyImageView> ImageView;

		vk::Image createImage(Rectangle size);
		vk::DeviceMemory allocateMemory();
		vk::ImageView createImageView();
	};
}
