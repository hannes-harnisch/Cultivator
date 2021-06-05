#pragma once

#include "PCH.hh"

#include "Utils/Rectangle.hh"
#include "Vendor/Vulkan/Vulkan.Unique.hh"

namespace ct
{
	class Texture final
	{
	public:
		Texture(Rectangle size);

		vk::ImageView imageView()
		{
			return imgView;
		}

	private:
		DeviceUnique<vk::Image, &vk::Device::destroyImage> image;
		DeviceUnique<vk::DeviceMemory, &vk::Device::freeMemory> memory;
		DeviceUnique<vk::Sampler, &vk::Device::destroySampler> sampler;
		DeviceUnique<vk::ImageView, &vk::Device::destroyImageView> imgView;

		vk::Image makeImage(Rectangle size);
		vk::DeviceMemory allocateMemory();
		vk::Sampler makeSampler();
		vk::ImageView makeImageView();
	};
}
