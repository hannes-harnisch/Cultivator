#pragma once

#include "PCH.hh"
#include "Utils/Rectangle.hh"
#include "Vendor/Vulkan/Vulkan.Surface.hh"

namespace ct::vulkan
{
	class SwapChain final
	{
	public:
		SwapChain(void* nativeWindowHandle, Rectangle viewport);
		SwapChain(SwapChain&& other) noexcept;
		~SwapChain();
		SwapChain& operator=(SwapChain&& other) noexcept;

		inline vk::Format getImageFormat() const
		{
			return SurfaceFormat.format;
		}

	private:
		Surface Surface;
		vk::SurfaceFormatKHR SurfaceFormat;
		vk::PresentModeKHR PresentMode;
		vk::Extent2D Extent;
		vk::SwapchainKHR SwapChainHandle;
		std::vector<vk::Image> SwapChainImages;
		std::vector<vk::ImageView> SwapChainViews;

		vk::SurfaceFormatKHR createSurfaceFormat();
		vk::PresentModeKHR createPresentMode();
		vk::Extent2D createExtent(Rectangle viewport);
		vk::SwapchainKHR createSwapChain();
		vk::SwapchainCreateInfoKHR fillSwapChainInfo();
		std::vector<vk::Image> createSwapChainImages();
		std::vector<vk::ImageView> createSwapChainImageViews();
	};
}
