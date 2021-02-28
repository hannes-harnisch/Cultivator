﻿#include "PCH.hh"
#include "Vulkan.SwapChain.hh"

#include "Utils/Assert.hh"
#include "Vendor/Vulkan/Vulkan.GraphicsContext.hh"

namespace ct::vulkan
{
	SwapChain::SwapChain(void* nativeWindowHandle, Rectangle viewport) :
		Surface {nativeWindowHandle},
		SurfaceFormat {createSurfaceFormat()},
		PresentMode {createPresentMode()},
		Extent {createExtent(viewport)},
		SwapChainHandle {createSwapChain()},
		SwapChainImages {createSwapChainImages()},
		SwapChainViews {createSwapChainImageViews()}
	{}

	SwapChain::SwapChain(SwapChain&& other) noexcept :
		Surface {std::move(other.Surface)},
		SurfaceFormat {other.SurfaceFormat},
		PresentMode {other.PresentMode},
		Extent {other.Extent},
		SwapChainHandle {std::exchange(other.SwapChainHandle, nullptr)},
		SwapChainImages {std::move(other.SwapChainImages)},
		SwapChainViews {std::move(other.SwapChainViews)}
	{}

	SwapChain::~SwapChain()
	{
		auto device {GraphicsContext::device()};

		for(auto imageView : SwapChainViews)
			device.destroyImageView(imageView, {}, Loader::get());

		device.destroySwapchainKHR(SwapChainHandle, {}, Loader::get());
	}

	SwapChain& SwapChain::operator=(SwapChain&& other) noexcept
	{
		Surface		  = std::move(other.Surface);
		SurfaceFormat = other.SurfaceFormat;
		PresentMode	  = other.PresentMode;
		Extent		  = other.Extent;
		std::swap(SwapChainHandle, other.SwapChainHandle);
		SwapChainImages = std::move(other.SwapChainImages);
		SwapChainViews	= std::move(other.SwapChainViews);
		return *this;
	}

	vk::SurfaceFormatKHR SwapChain::createSurfaceFormat()
	{
		auto [res, formats] {GraphicsContext::adapter().getSurfaceFormatsKHR(Surface.handle(), Loader::get())};
		ctEnsureResult(res, "Failed to create Vulkan surface formats.");

		for(auto format : formats)
			if(format == vk::SurfaceFormatKHR(vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear))
				return format;

		return formats[0];
	}

	vk::PresentModeKHR SwapChain::createPresentMode()
	{
		auto [res, modes] {GraphicsContext::adapter().getSurfacePresentModesKHR(Surface.handle(), Loader::get())};
		ctEnsureResult(res, "Failed to create Vulkan surface presentation modes.");

		for(auto mode : modes)
			if(mode == vk::PresentModeKHR::eMailbox)
				return mode;

		return vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D SwapChain::createExtent(Rectangle viewport)
	{
		auto [res, caps] {GraphicsContext::adapter().getSurfaceCapabilitiesKHR(Surface.handle(), Loader::get())};
		ctEnsureResult(res, "Failed to create Vulkan surface capabilities.");

		uint32_t imageCount {caps.minImageCount + 1};
		if(caps.maxImageCount > 0 && imageCount > caps.maxImageCount)
			imageCount = caps.maxImageCount;
		SwapChainImages.resize(imageCount);

		if(caps.currentExtent != UINT32_MAX)
			return caps.currentExtent;
		else
			return {std::clamp(viewport.Width, caps.minImageExtent.width, caps.maxImageExtent.width),
					std::clamp(viewport.Height, caps.minImageExtent.height, caps.maxImageExtent.height)};
	}

	vk::SwapchainKHR SwapChain::createSwapChain()
	{
		ctEnsure(GraphicsContext::presentQueue().supportsSurface(Surface), "Vulkan surface is unsuitable.");
		auto swapChainInfo {fillSwapChainInfo()};

		const uint32_t graphicsFamily {GraphicsContext::graphicsQueue().familyIndex()};
		const uint32_t presentFamily {GraphicsContext::presentQueue().familyIndex()};
		if(graphicsFamily != presentFamily)
		{
			std::array indices {graphicsFamily, presentFamily};
			swapChainInfo.setImageSharingMode(vk::SharingMode::eConcurrent).setQueueFamilyIndices(indices);
		}

		auto [res, swapChain] {GraphicsContext::device().createSwapchainKHR(swapChainInfo, nullptr, Loader::get())};
		ctEnsureResult(res, "Failed to create Vulkan swap chain.");
		return swapChain;
	}

	vk::SwapchainCreateInfoKHR SwapChain::fillSwapChainInfo()
	{
		return vk::SwapchainCreateInfoKHR()
			.setSurface(Surface.handle())
			.setMinImageCount(uint32_t(SwapChainImages.size()))
			.setImageFormat(SurfaceFormat.format)
			.setImageColorSpace(SurfaceFormat.colorSpace)
			.setImageExtent(Extent)
			.setImageArrayLayers(1)
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
			.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity)
			.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
			.setPresentMode(PresentMode)
			.setClipped(true);
	}

	std::vector<vk::Image> SwapChain::createSwapChainImages()
	{
		auto [res, images] {GraphicsContext::device().getSwapchainImagesKHR(SwapChainHandle, Loader::get())};
		ctEnsureResult(res, "Failed to get swap chain images.");
		return images;
	}

	std::vector<vk::ImageView> SwapChain::createSwapChainImageViews()
	{
		std::vector<vk::ImageView> views(SwapChainImages.size());
		for(auto image : SwapChainImages)
		{
			auto subresourceRange {vk::ImageSubresourceRange()
									   .setAspectMask(vk::ImageAspectFlagBits::eColor)
									   .setLevelCount(1)
									   .setLayerCount(1)};
			auto imageViewInfo {vk::ImageViewCreateInfo()
									.setImage(image)
									.setViewType(vk::ImageViewType::e2D)
									.setFormat(SurfaceFormat.format)
									.setSubresourceRange(subresourceRange)};
			auto [res, imageView] {GraphicsContext::device().createImageView(imageViewInfo, nullptr, Loader::get())};
			ctEnsureResult(res, "Failed to create swap chain image views.");
			views.push_back(imageView);
		}
		return views;
	}
}
