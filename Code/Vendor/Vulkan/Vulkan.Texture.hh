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

		vk::ImageView imageView() const
		{
			return imgView;
		}

	private:
		DeviceOwn<vk::Image, &vk::Device::destroyImage> image;
		DeviceOwn<vk::DeviceMemory, &vk::Device::freeMemory> memory;
		DeviceOwn<vk::Sampler, &vk::Device::destroySampler> sampler;
		DeviceOwn<vk::ImageView, &vk::Device::destroyImageView> imgView;

		vk::Image makeImage(Rectangle size);
		vk::DeviceMemory allocateMemory();
		vk::ImageView makeImageView();
	};
}
