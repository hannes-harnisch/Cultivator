#pragma once

#include "PCH.hh"
#include "Rectangle.hh"
#include "Vendor/Vulkan/Vulkan.Surface.hh"

namespace ct::vulkan
{
	class SwapChain final
	{
	public:
		SwapChain(void* windowHandle, Rectangle viewport);
		SwapChain(SwapChain&& other) noexcept;
		~SwapChain();
		SwapChain& operator=(SwapChain&& other) noexcept;

	private:
		Surface Surface;
		vk::SurfaceFormatKHR SurfaceFormat;
		vk::PresentModeKHR PresentMode;
		vk::Extent2D Extent;
		vk::SwapchainKHR SwapChainHandle;
		std::vector<vk::Image> SwapChainImages;
		std::vector<vk::ImageView> SwapChainViews;

		vk::SurfaceFormatKHR querySurfaceFormat();
		vk::PresentModeKHR queryPresentMode();
		vk::Extent2D queryExtent(Rectangle viewport);
		vk::SwapchainKHR querySwapChain();
		vk::SwapchainCreateInfoKHR fillSwapChainInfo();
		std::vector<vk::Image> querySwapChainImages();
		std::vector<vk::ImageView> querySwapChainImageViews();
	};
}
