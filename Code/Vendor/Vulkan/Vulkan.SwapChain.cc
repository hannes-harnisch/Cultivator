#include "PCH.hh"
#include "Vulkan.SwapChain.hh"

#include "Assert.hh"
#include "Vendor/Vulkan/Vulkan.GraphicsPlatform.hh"

namespace ct::vulkan
{
	SwapChain::SwapChain(void* windowHandle, Rectangle viewport) :
		Surface(windowHandle), SurfaceFormat(querySurfaceFormat()), PresentMode(queryPresentMode()),
		Extent(queryExtent(viewport)), SwapChainHandle(querySwapChain())
	{}

	SwapChain::SwapChain(SwapChain&& other) noexcept :
		Surface(std::move(other.Surface)), SwapChainHandle(std::exchange(other.SwapChainHandle, nullptr))
	{}

	SwapChain::~SwapChain()
	{
		GraphicsPlatform::get().device().destroySwapchainKHR(SwapChainHandle);
	}

	SwapChain& SwapChain::operator=(SwapChain&& other) noexcept
	{
		Surface = std::move(other.Surface);
		std::swap(SwapChainHandle, other.SwapChainHandle);
		return *this;
	}

	vk::SurfaceFormatKHR SwapChain::querySurfaceFormat()
	{
		auto formats {GraphicsPlatform::get().adapter().getSurfaceFormatsKHR(Surface.handle())};
		ctEnsureResult(formats.result, "Failed to query Vulkan surface formats.");

		for(auto format : formats.value)
			if(format == vk::SurfaceFormatKHR(vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear))
				return format;

		return formats.value[0];
	}

	vk::PresentModeKHR SwapChain::queryPresentMode()
	{
		auto modes {GraphicsPlatform::get().adapter().getSurfacePresentModesKHR(Surface.handle())};
		ctEnsureResult(modes.result, "Failed to query Vulkan surface presentation modes.");

		for(auto mode : modes.value)
			if(mode == vk::PresentModeKHR::eMailbox)
				return mode;

		return vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D SwapChain::queryExtent(Rectangle viewport)
	{
		auto capabilities {GraphicsPlatform::get().adapter().getSurfaceCapabilitiesKHR(Surface.handle())};
		ctEnsureResult(capabilities.result, "Failed to query Vulkan surface capabilities.");
		auto& caps {capabilities.value};

		uint32_t imageCount {std::min(caps.minImageCount + 1, caps.maxImageCount)};
		SwapChainImages.resize(imageCount);

		if(caps.currentExtent != UINT32_MAX)
			return caps.currentExtent;
		else
			return {std::clamp(viewport.Width, caps.minImageExtent.width, caps.maxImageExtent.width),
					std::clamp(viewport.Height, caps.minImageExtent.height, caps.maxImageExtent.height)};
	}

	vk::SwapchainKHR SwapChain::querySwapChain()
	{
		auto swapChainInfo {vk::SwapchainCreateInfoKHR()
								.setSurface(Surface.handle())
								.setMinImageCount(SwapChainImages.size())
								.setImageFormat(SurfaceFormat.format)
								.setImageColorSpace(SurfaceFormat.colorSpace)
								.setImageExtent(Extent)
								.setImageArrayLayers(1)
								.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
								// TODO: differentiate queues
								.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity)
								.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
								.setPresentMode(PresentMode)
								.setClipped(true)};

		auto swapChain {GraphicsPlatform::get().device().createSwapchainKHR(swapChainInfo)};
		ctEnsureResult(swapChain.result, "Failed to create Vulkan swap chain.");
		return swapChain.value;
	}

}
