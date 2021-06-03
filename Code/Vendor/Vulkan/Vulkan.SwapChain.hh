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
			return surfaceFormat.format;
		}

		void present();

	private:
		Surface surface;
		vk::SurfaceFormatKHR surfaceFormat;
		vk::PresentModeKHR presentMode;
		vk::Extent2D extent;
		DeviceUnique<vk::SwapchainKHR, &vk::Device::destroySwapchainKHR> swapChain;
		std::vector<vk::Image> swapChainImages;
		std::vector<DeviceUnique<vk::ImageView, &vk::Device::destroyImageView>> swapChainViews;

		vk::SurfaceFormatKHR makeSurfaceFormat();
		vk::PresentModeKHR makePresentMode();
		vk::Extent2D makeExtent(Rectangle viewport);
		vk::SwapchainKHR makeSwapChain();
		vk::SwapchainCreateInfoKHR fillSwapChainInfo();
		std::vector<vk::Image> makeSwapChainImages();
		std::vector<DeviceUnique<vk::ImageView, &vk::Device::destroyImageView>> makeSwapChainImageViews();
	};
}
