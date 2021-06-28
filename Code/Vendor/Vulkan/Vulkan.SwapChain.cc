#include "PCH.hh"

#include "Utils/Assert.hh"
#include "Vulkan.GPUContext.hh"
#include "Vulkan.SwapChain.hh"
#include "Vulkan.Utils.hh"

namespace ct
{
	SwapChain::SwapChain(void* const nativeWindowHandle, Rectangle const viewport, RenderPass const& renderPass) :
		surface(nativeWindowHandle),
		surfaceFormat(makeSurfaceFormat()),
		presentMode(makePresentMode()),
		extent(makeExtent(viewport)),
		swapChain(makeSwapChain()),
		swapChainImages(makeSwapChainImages()),
		swapChainViews(makeSwapChainImageViews()),
		frameBuffers(makeFrameBuffers(renderPass))
	{}

	uint32_t SwapChain::getNextImageIndex(vk::Semaphore imgGetSemaphore)
	{
		auto [res, imgIndex] = GPUContext::device().acquireNextImageKHR(swapChain, UINT64_MAX, imgGetSemaphore, nullptr);
		if(res == vk::Result::eErrorOutOfDateKHR)
			std::exit(EXIT_FAILURE);

		return imgIndex;
	}

	void SwapChain::present(uint32_t imageIndex, vk::Semaphore imgDoneSemaphore)
	{
		auto chain = swapChain.get();
		vk::PresentInfoKHR info {
			.waitSemaphoreCount = 1,
			.pWaitSemaphores	= &imgDoneSemaphore,
			.swapchainCount		= 1,
			.pSwapchains		= &chain,
			.pImageIndices		= &imageIndex,
		};
		auto result = GPUContext::presentQueue().handle().presentKHR(info);

		if(result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
			std::exit(EXIT_FAILURE);
	}

	vk::SurfaceFormatKHR SwapChain::makeSurfaceFormat()
	{
		auto [res, formats] = GPUContext::adapter().getSurfaceFormatsKHR(surface.handle());
		ctEnsureResult(res, "Failed to create Vulkan surface formats.");

		for(auto format : formats)
			if(format == vk::SurfaceFormatKHR(vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear))
				return format;

		return formats[0];
	}

	vk::PresentModeKHR SwapChain::makePresentMode()
	{
		auto [res, modes] = GPUContext::adapter().getSurfacePresentModesKHR(surface.handle());
		ctEnsureResult(res, "Failed to create Vulkan surface presentation modes.");

		for(auto mode : modes)
			if(mode == vk::PresentModeKHR::eMailbox)
				return mode;

		return vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D SwapChain::makeExtent(Rectangle const viewport)
	{
		auto [res, caps] = GPUContext::adapter().getSurfaceCapabilitiesKHR(surface.handle());
		ctEnsureResult(res, "Failed to create Vulkan surface capabilities.");

		uint32_t imageCount = caps.minImageCount + 1;
		if(caps.maxImageCount > 0 && imageCount > caps.maxImageCount)
			imageCount = caps.maxImageCount;

		swapChainImages.resize(imageCount);

		if(caps.currentExtent != vk::Extent2D(UINT32_MAX, UINT32_MAX))
			return caps.currentExtent;
		else
			return {std::clamp(viewport.width, caps.minImageExtent.width, caps.maxImageExtent.width),
					std::clamp(viewport.height, caps.minImageExtent.height, caps.maxImageExtent.height)};
	}

	vk::SwapchainKHR SwapChain::makeSwapChain()
	{
		ctEnsure(GPUContext::presentQueue().supportsSurface(surface), "Vulkan surface is unsuitable.");
		auto swapChainInfo = fillSwapChainInfo();

		uint32_t const graphicsFamily = GPUContext::graphicsQueue().getFamily();
		uint32_t const presentFamily  = GPUContext::presentQueue().getFamily();
		std::array indices {graphicsFamily, presentFamily};
		if(graphicsFamily != presentFamily)
		{
			swapChainInfo.imageSharingMode		= vk::SharingMode::eConcurrent;
			swapChainInfo.queueFamilyIndexCount = count(indices);
			swapChainInfo.pQueueFamilyIndices	= indices.data();
		}

		auto [res, handle] = GPUContext::device().createSwapchainKHR(swapChainInfo);
		ctEnsureResult(res, "Failed to create Vulkan swap chain.");
		return handle;
	}

	vk::SwapchainCreateInfoKHR SwapChain::fillSwapChainInfo()
	{
		return vk::SwapchainCreateInfoKHR {
			.surface		  = surface.handle(),
			.minImageCount	  = count(swapChainImages),
			.imageFormat	  = surfaceFormat.format,
			.imageColorSpace  = surfaceFormat.colorSpace,
			.imageExtent	  = extent,
			.imageArrayLayers = 1,
			.imageUsage		  = vk::ImageUsageFlagBits::eColorAttachment,
			.preTransform	  = vk::SurfaceTransformFlagBitsKHR::eIdentity,
			.compositeAlpha	  = vk::CompositeAlphaFlagBitsKHR::eOpaque,
			.presentMode	  = presentMode,
			.clipped		  = true,
		};
	}

	std::vector<vk::Image> SwapChain::makeSwapChainImages()
	{
		auto [res, images] = GPUContext::device().getSwapchainImagesKHR(swapChain);
		ctEnsureResult(res, "Failed to get swap chain images.");
		return images;
	}

	std::vector<DeviceOwn<vk::ImageView, &vk::Device::destroyImageView>> SwapChain::makeSwapChainImageViews()
	{
		std::vector<DeviceOwn<vk::ImageView, &vk::Device::destroyImageView>> views;
		for(auto image : swapChainImages)
		{
			vk::ImageSubresourceRange subresourceRange {
				.aspectMask = vk::ImageAspectFlagBits::eColor,
				.levelCount = 1,
				.layerCount = 1,
			};
			vk::ImageViewCreateInfo imageViewInfo {
				.image			  = image,
				.viewType		  = vk::ImageViewType::e2D,
				.format			  = surfaceFormat.format,
				.subresourceRange = subresourceRange,
			};
			auto [res, imageView] = GPUContext::device().createImageView(imageViewInfo);
			ctEnsureResult(res, "Failed to create swap chain image views.");
			views.emplace_back(imageView);
		}
		return views;
	}

	std::vector<FrameBuffer> SwapChain::makeFrameBuffers(RenderPass const& renderPass)
	{
		std::vector<FrameBuffer> buffers;

		for(vk::ImageView view : swapChainViews)
			buffers.emplace_back(Rectangle {extent.width, extent.height}, renderPass, view);

		return buffers;
	}
}
