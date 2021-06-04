#include "PCH.hh"

#include "Utils/Assert.hh"
#include "Vendor/Vulkan/Vulkan.GPUContext.hh"
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
	{
		auto fenceInfo = vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled);
		vk::SemaphoreCreateInfo semaphoreInfo;
		for(int i {}; i < MaxFrames; ++i)
		{
			auto [getSemRes, imageGetSemaphore] = GPUContext::device().createSemaphore(semaphoreInfo, nullptr, Loader::get());
			ctEnsureResult(getSemRes, "Failed to create image-get semaphore.");
			imgGetSemaphores[i] = imageGetSemaphore;

			auto [doneSemRes, imageDoneSemaphore] = GPUContext::device().createSemaphore(semaphoreInfo, nullptr, Loader::get());
			ctEnsureResult(doneSemRes, "Failed to create image-done semaphore.");
			imgDoneSemaphores[i] = imageDoneSemaphore;

			auto [fenceRes, fence] = GPUContext::device().createFence(fenceInfo, nullptr, Loader::get());
			ctEnsureResult(fenceRes, "Failed to create fence.");
			frameFences[i] = fence;
		}
		imgInFlightFences.resize(swapChainImages.size());
	}

	uint32_t SwapChain::getNextImageIndex()
	{
		std::array fence {frameFences[currentFrame].get()};
		GPUContext::device().waitForFences(fence, true, UINT64_MAX, Loader::get());

		auto [res, imgIndex] = GPUContext::device().acquireNextImageKHR(swapChain, UINT64_MAX, imgGetSemaphores[currentFrame],
																		nullptr, Loader::get());
		if(res == vk::Result::eErrorOutOfDateKHR)
			throw "Swap chain resize not implemented.";

		if(imgInFlightFences[imgIndex])
		{
			std::array fence {imgInFlightFences[imgIndex]};
			ctAssertResult(GPUContext::device().waitForFences(fence, true, UINT64_MAX, Loader::get()),
						   "Failed to wait for fences.");
		}
		imgInFlightFences[imgIndex] = frameFences[currentFrame];
		GPUContext::device().resetFences(fence, Loader::get());

		return imgIndex;
	}

	void SwapChain::present(uint32_t imageIndex, vk::Semaphore semaphore)
	{
		std::array waitSemaphores {imgGetSemaphores[currentFrame].get()};
		std::array signalSemaphores {imgDoneSemaphores[currentFrame].get()};
		std::array<vk::PipelineStageFlags, 1> waitStages {vk::PipelineStageFlagBits::eColorAttachmentOutput};
		auto submit = vk::SubmitInfo()
						  .setWaitSemaphores(waitSemaphores)
						  .setWaitDstStageMask(waitStages)
						  .setCommandBuffers()
						  .setSignalSemaphores(signalSemaphores);
		// queue Submit
		std::array swapChains {swapChain.get()};
		std::array imageDoneSemaphore {semaphore};

		auto presentInfo =
			vk::PresentInfoKHR().setWaitSemaphores(imageDoneSemaphore).setSwapchains(swapChains).setPImageIndices(&imageIndex);
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

		if(caps.currentExtent != UINT32_MAX)
			return caps.currentExtent;
		else
			return {std::clamp(viewport.Width, caps.minImageExtent.width, caps.maxImageExtent.width),
					std::clamp(viewport.Height, caps.minImageExtent.height, caps.maxImageExtent.height)};
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

	std::vector<DeviceUnique<vk::ImageView, &vk::Device::destroyImageView>> SwapChain::makeSwapChainImageViews()
	{
		std::vector<DeviceUnique<vk::ImageView, &vk::Device::destroyImageView>> views(swapChainImages.size());
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
