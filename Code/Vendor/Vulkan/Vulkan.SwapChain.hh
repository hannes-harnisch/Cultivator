#pragma once

#include "PCH.hh"

#include "Utils/Rectangle.hh"
#include "Vulkan.Surface.hh"
#include "Vulkan.Unique.hh"

namespace ct
{
	class SwapChain final
	{
	public:
		SwapChain(void* nativeWindowHandle, Rectangle viewport);

		size_t getImageCount() const
		{
			return swapChainImages.size();
		}

		vk::Format getImageFormat() const
		{
			return surfaceFormat.format;
		}

		uint32_t getNextImageIndex(vk::Semaphore imgGetSemaphore);
		void present(uint32_t imageIndex, vk::Semaphore imgDoneSemaphore);

	private:
		Surface surface;
		vk::SurfaceFormatKHR surfaceFormat;
		vk::PresentModeKHR presentMode;
		vk::Extent2D extent;
		DeviceOwn<vk::SwapchainKHR, &vk::Device::destroySwapchainKHR> swapChain;
		std::vector<vk::Image> swapChainImages;
		std::vector<DeviceOwn<vk::ImageView, &vk::Device::destroyImageView>> swapChainViews;

		vk::SurfaceFormatKHR makeSurfaceFormat();
		vk::PresentModeKHR makePresentMode();
		vk::Extent2D makeExtent(Rectangle viewport);
		vk::SwapchainKHR makeSwapChain();
		vk::SwapchainCreateInfoKHR fillSwapChainInfo();
		std::vector<vk::Image> makeSwapChainImages();
		std::vector<DeviceOwn<vk::ImageView, &vk::Device::destroyImageView>> makeSwapChainImageViews();
	};
}
