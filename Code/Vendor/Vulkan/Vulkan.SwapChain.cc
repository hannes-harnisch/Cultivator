#include "PCH.hh"

#include "Utils/Assert.hh"
#include "Vulkan.GPUContext.hh"
#include "Vulkan.SwapChain.hh"

namespace ct
{
	SwapChain::SwapChain(void* const nativeWindowHandle, Rectangle const viewport) :
		surface(nativeWindowHandle),
		surfaceFormat(makeSurfaceFormat()),
		presentMode(makePresentMode()),
		extent(makeExtent(viewport)),
		swapChain(makeSwapChain()),
		swapChainImages(makeSwapChainImages()),
		swapChainViews(makeSwapChainImageViews())
	{}

	uint32_t SwapChain::getNextImageIndex(vk::Semaphore imgGetSemaphore)
	{
		auto [res, imgIndex] = GPUContext::device().acquireNextImageKHR(swapChain, std::numeric_limits<uint64_t>::max(),
																		imgGetSemaphore, nullptr, Loader::get());
		if(res == vk::Result::eErrorOutOfDateKHR)
			throw "Swap chain resize not implemented.";

		return imgIndex;
	}

	void SwapChain::present(uint32_t imageIndex, vk::Semaphore imgDoneSemaphore)
	{
		std::array swapChains {swapChain.get()};
		std::array imageDoneSemaphores {imgDoneSemaphore};

		auto presentInfo =
			vk::PresentInfoKHR().setWaitSemaphores(imageDoneSemaphores).setSwapchains(swapChains).setPImageIndices(&imageIndex);
		auto result = GPUContext::presentQueue().handle().presentKHR(presentInfo, Loader::get());

		if(result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
			throw "Swap chain resize not handled.";
	}

	vk::SurfaceFormatKHR SwapChain::makeSurfaceFormat()
	{
		auto [res, formats] = GPUContext::adapter().getSurfaceFormatsKHR(surface.handle(), Loader::get());
		ctEnsureResult(res, "Failed to create Vulkan surface formats.");

		for(auto format : formats)
			if(format == vk::SurfaceFormatKHR(vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear))
				return format;

		return formats[0];
	}

	vk::PresentModeKHR SwapChain::makePresentMode()
	{
		auto [res, modes] = GPUContext::adapter().getSurfacePresentModesKHR(surface.handle(), Loader::get());
		ctEnsureResult(res, "Failed to create Vulkan surface presentation modes.");

		for(auto mode : modes)
			if(mode == vk::PresentModeKHR::eMailbox)
				return mode;

		return vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D SwapChain::makeExtent(Rectangle const viewport)
	{
		auto [res, caps] = GPUContext::adapter().getSurfaceCapabilitiesKHR(surface.handle(), Loader::get());
		ctEnsureResult(res, "Failed to create Vulkan surface capabilities.");

		uint32_t imageCount = caps.minImageCount + 1;
		if(caps.maxImageCount > 0 && imageCount > caps.maxImageCount)
			imageCount = caps.maxImageCount;
		swapChainImages.resize(imageCount);

		if(caps.currentExtent != std::numeric_limits<uint32_t>::max())
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
		if(graphicsFamily != presentFamily)
		{
			std::array indices {graphicsFamily, presentFamily};
			swapChainInfo.setImageSharingMode(vk::SharingMode::eConcurrent).setQueueFamilyIndices(indices);
		}

		auto [res, handle] = GPUContext::device().createSwapchainKHR(swapChainInfo, nullptr, Loader::get());
		ctEnsureResult(res, "Failed to create Vulkan swap chain.");
		return handle;
	}

	vk::SwapchainCreateInfoKHR SwapChain::fillSwapChainInfo()
	{
		return vk::SwapchainCreateInfoKHR()
			.setSurface(surface.handle())
			.setMinImageCount(uint32_t(swapChainImages.size()))
			.setImageFormat(surfaceFormat.format)
			.setImageColorSpace(surfaceFormat.colorSpace)
			.setImageExtent(extent)
			.setImageArrayLayers(1)
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
			.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity)
			.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
			.setPresentMode(presentMode)
			.setClipped(true);
	}

	std::vector<vk::Image> SwapChain::makeSwapChainImages()
	{
		auto [res, images] = GPUContext::device().getSwapchainImagesKHR(swapChain, Loader::get());
		ctEnsureResult(res, "Failed to get swap chain images.");
		return images;
	}

	std::vector<DeviceOwn<vk::ImageView, &vk::Device::destroyImageView>> SwapChain::makeSwapChainImageViews()
	{
		std::vector<DeviceOwn<vk::ImageView, &vk::Device::destroyImageView>> views(swapChainImages.size());
		for(auto image : swapChainImages)
		{
			auto subresourceRange =
				vk::ImageSubresourceRange().setAspectMask(vk::ImageAspectFlagBits::eColor).setLevelCount(1).setLayerCount(1);
			auto imageViewInfo = vk::ImageViewCreateInfo()
									 .setImage(image)
									 .setViewType(vk::ImageViewType::e2D)
									 .setFormat(surfaceFormat.format)
									 .setSubresourceRange(subresourceRange);
			auto [res, imageView] = GPUContext::device().createImageView(imageViewInfo, nullptr, Loader::get());
			ctEnsureResult(res, "Failed to create swap chain image views.");
			views.emplace_back(imageView);
		}
		return views;
	}
}
