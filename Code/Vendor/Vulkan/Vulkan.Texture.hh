#pragma once

#include "PCH.hh"

#include "Utils/Rectangle.hh"
#include "Vulkan.Unique.hh"

namespace ct
{
	class Texture final
	{
	public:
		Texture(Rectangle size);

		Rectangle size() const
		{
			return texSize;
		}

		vk::Image image() const
		{
			return img;
		}

		vk::ImageView imageView() const
		{
			return imgView;
		}

	private:
		Rectangle texSize;
		DeviceOwn<vk::DeviceMemory, &vk::Device::freeMemory> memory;
		DeviceOwn<vk::Image, &vk::Device::destroyImage> img;
		DeviceOwn<vk::ImageView, &vk::Device::destroyImageView> imgView;

		vk::Image makeImage(Rectangle size);
		vk::DeviceMemory allocateMemory();
		vk::ImageView makeImageView();
	};
}
