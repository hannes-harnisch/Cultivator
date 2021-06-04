#pragma once

#include "PCH.hh"

#include "Utils/Rectangle.hh"
#include "Vendor/Vulkan/Vulkan.Surface.hh"
#include "Vendor/Vulkan/Vulkan.Unique.hh"

namespace ct
{
	class SwapChain final
	{
	public:
		SwapChain(void* nativeWindowHandle, Rectangle viewport);

		vk::Format getImageFormat() const
		{
			return surfaceFormat.format;
		}

		uint32_t getNextImageIndex();
		void present(uint32_t imageIndex, vk::Semaphore semaphore);

	private:
		constexpr static uint32_t MaxFrames = 2;

		Surface surface;
		vk::SurfaceFormatKHR surfaceFormat;
		vk::PresentModeKHR presentMode;
		vk::Extent2D extent;
		DeviceUnique<vk::SwapchainKHR, &vk::Device::destroySwapchainKHR> swapChain;
		std::vector<vk::Image> swapChainImages;
		std::vector<DeviceUnique<vk::ImageView, &vk::Device::destroyImageView>> swapChainViews;
		std::vector<vk::Fence> imgInFlightFences;
		std::array<DeviceUnique<vk::Fence, &vk::Device::destroyFence>, MaxFrames> frameFences;
		std::array<DeviceUnique<vk::Semaphore, &vk::Device::destroySemaphore>, MaxFrames> imgDoneSemaphores;
		std::array<DeviceUnique<vk::Semaphore, &vk::Device::destroySemaphore>, MaxFrames> imgGetSemaphores;
		uint32_t currentFrame {};

		vk::SurfaceFormatKHR makeSurfaceFormat();
		vk::PresentModeKHR makePresentMode();
		vk::Extent2D makeExtent(Rectangle viewport);
		vk::SwapchainKHR makeSwapChain();
		vk::SwapchainCreateInfoKHR fillSwapChainInfo();
		std::vector<vk::Image> makeSwapChainImages();
		std::vector<DeviceUnique<vk::ImageView, &vk::Device::destroyImageView>> makeSwapChainImageViews();
	};
}
