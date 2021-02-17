#include "PCH.hh"
#include "Vulkan.SwapChain.hh"

#include "Assert.hh"
#include "Vendor/Vulkan/Vulkan.GraphicsPlatform.hh"

namespace ct::vulkan
{
	SwapChain::SwapChain(void* windowHandle, Rectangle viewport) :
		Surface {windowHandle},
		SurfaceFormat {querySurfaceFormat()},
		PresentMode {queryPresentMode()},
		Extent {queryExtent(viewport)},
		SwapChainHandle {querySwapChain()},
		SwapChainImages {querySwapChainImages()},
		SwapChainViews {querySwapChainImageViews()}
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
		auto device {GraphicsPlatform::get().device()};

		for(auto imageView : SwapChainViews)
			device.destroyImageView(imageView);

		device.destroySwapchainKHR(SwapChainHandle);
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
		ctEnsure(GraphicsPlatform::get().presentQueue().supportsSurface(Surface), "Vulkan surface is unsuitable.");
		auto swapChainInfo {fillSwapChainInfo()};

		const uint32_t graphicsIndex {GraphicsPlatform::get().graphicsQueue().familyIndex()};
		const uint32_t presentIndex {GraphicsPlatform::get().presentQueue().familyIndex()};
		if(graphicsIndex != presentIndex)
		{
			const uint32_t indices[] {graphicsIndex, presentIndex};
			swapChainInfo.setImageSharingMode(vk::SharingMode::eConcurrent)
				.setQueueFamilyIndexCount(uint32_t(std::size(indices)))
				.setPQueueFamilyIndices(indices);
		}
		else
			swapChainInfo.setImageSharingMode(vk::SharingMode::eExclusive);

		auto swapChain {GraphicsPlatform::get().device().createSwapchainKHR(swapChainInfo)};
		ctEnsureResult(swapChain.result, "Failed to create Vulkan swap chain.");
		return swapChain.value;
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

	std::vector<vk::Image> SwapChain::querySwapChainImages()
	{
		auto images {GraphicsPlatform::get().device().getSwapchainImagesKHR(SwapChainHandle)};
		ctEnsureResult(images.result, "Failed to get swap chain images.");
		return images.value;
	}

	std::vector<vk::ImageView> SwapChain::querySwapChainImageViews()
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
			auto imageView {GraphicsPlatform::get().device().createImageView(imageViewInfo)};
			ctEnsureResult(imageView.result, "Failed to create swap chain image views.");
			views.push_back(imageView.value);
		}
		return views;
	}
}
