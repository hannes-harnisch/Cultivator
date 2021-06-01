#pragma once

#include "PCH.hh"

#include "Utils/Rectangle.hh"
#include "Vendor/Vulkan/Vulkan.Surface.hh"
#include "Vendor/Vulkan/Vulkan.Unique.hh"

namespace ct::vulkan
{
	class SwapChain final
	{
	public:
		SwapChain(void* nativeWindowHandle, Rectangle viewport);

		vk::Format getImageFormat() const
		{
			return SurfaceFormat.format;
		}

	private:
		Surface Surface;
		vk::SurfaceFormatKHR SurfaceFormat;
		vk::PresentModeKHR PresentMode;
		vk::Extent2D Extent;
		DeviceUnique<vk::SwapchainKHR, &vk::Device::destroySwapchainKHR> SwapChainHandle;
		std::vector<vk::Image> SwapChainImages;
		std::vector<DeviceUnique<vk::ImageView, &vk::Device::destroyImageView>> SwapChainViews;

		vk::SurfaceFormatKHR createSurfaceFormat();
		vk::PresentModeKHR createPresentMode();
		vk::Extent2D createExtent(Rectangle viewport);
		vk::SwapchainKHR createSwapChain();
		vk::SwapchainCreateInfoKHR fillSwapChainInfo();
		std::vector<vk::Image> createSwapChainImages();
		std::vector<DeviceUnique<vk::ImageView, &vk::Device::destroyImageView>> createSwapChainImageViews();
	};
}
