#include "SwapChain.hpp"

#include "graphics/vulkan/Util.hpp"
#include "util/Util.hpp"

namespace cltv {

SwapChain::SwapChain(const DeviceContext* ctx, const Window* window, const RenderPass& render_pass) :
	ctx_(ctx),
	window_(window),
	surface_(ctx, *window),
	render_pass_(render_pass.get()) {
	init_surface_format();
	init_present_mode();
	init_extent_and_swapchain(window->get_size());
	init_images();
}

SwapChain::~SwapChain() {
	for (VkFramebuffer framebuffer : framebuffers_) {
		ctx_->lib.vkDestroyFramebuffer(ctx_->device(), framebuffer, nullptr);
	}

	// swapchain owns the VkImages, we don't need to destroy them

	for (VkImageView image_view : image_views_) {
		ctx_->lib.vkDestroyImageView(ctx_->device(), image_view, nullptr);
	}

	ctx_->lib.vkDestroySwapchainKHR(ctx_->device(), swapchain_, nullptr);
}

RectSize SwapChain::get_size() const {
	return RectSize {
		.width	= static_cast<int32_t>(extent_.width),
		.height = static_cast<int32_t>(extent_.height),
	};
}

std::optional<uint32_t> SwapChain::get_next_image_index(VkSemaphore img_acquire_semaphore) {
	uint32_t index;
	VkResult result = ctx_->lib.vkAcquireNextImageKHR(ctx_->device(), swapchain_, UINT64_MAX, img_acquire_semaphore,
													  VK_NULL_HANDLE, &index);
	if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR) {
		return index;
	} else if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreate();
		return std::nullopt;
	} else {
		fail_vk_result(result, "could not acquire next swapchain image");
	}
}

void SwapChain::present(uint32_t image_index, VkSemaphore img_release_semaphore) {
	VkPresentInfoKHR present_info {
		.sType				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext				= nullptr,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores	= &img_release_semaphore,
		.swapchainCount		= 1,
		.pSwapchains		= &swapchain_,
		.pImageIndices		= &image_index,
		.pResults			= nullptr,
	};
	VkResult result = ctx_->lib.vkQueuePresentKHR(ctx_->presentation_queue.queue, &present_info);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		recreate();
	} else if (result != VK_SUCCESS) {
		fail_vk_result(result, "could not present swapchain image to queue");
	}
}

void SwapChain::recreate() {
	RectSize size = window_->get_size();
	if (size.is_empty()) {
		return;
	}

	VkResult result = ctx_->lib.vkDeviceWaitIdle(ctx_->device());
	require_vk_result(result, "failed to wait for device idle when recreating swapchain");

	for (VkFramebuffer framebuffer : framebuffers_) {
		ctx_->lib.vkDestroyFramebuffer(ctx_->device(), framebuffer, nullptr);
	}
	framebuffers_.clear();

	// swapchain owns the VkImages, we don't need to destroy them
	images_.clear();

	for (VkImageView image_view : image_views_) {
		ctx_->lib.vkDestroyImageView(ctx_->device(), image_view, nullptr);
	}
	image_views_.clear();

	VkSwapchainKHR old_swapchain = swapchain_;

	init_extent_and_swapchain(size);
	init_images();

	// destroy old swapchain after it was reused for new one
	ctx_->lib.vkDestroySwapchainKHR(ctx_->device(), old_swapchain, nullptr);
}

void SwapChain::init_surface_format() {
	uint32_t count;
	VkResult result = ctx_->lib.vkGetPhysicalDeviceSurfaceFormatsKHR(ctx_->physical_device(), surface_.get(), &count, nullptr);
	require_vk_result(result, "failed to get physical device surface format count");

	std::vector<VkSurfaceFormatKHR> formats(count);
	result = ctx_->lib.vkGetPhysicalDeviceSurfaceFormatsKHR(ctx_->physical_device(), surface_.get(), &count, formats.data());
	require_vk_result(result, "failed to get physical device surface formats");

	static constexpr VkSurfaceFormatKHR Desired {
		.format		= VK_FORMAT_B8G8R8A8_SRGB,
		.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
	};
	auto equals_desired = [](VkSurfaceFormatKHR format) -> bool {
		return format.format == Desired.format && format.colorSpace == Desired.colorSpace;
	};
	if (std::ranges::find_if(formats, equals_desired) != formats.end()) {
		surface_format_ = Desired;
	} else {
		surface_format_ = formats.at(0);
	}
}

void SwapChain::init_present_mode() {
	uint32_t count;
	VkResult result = ctx_->lib.vkGetPhysicalDeviceSurfacePresentModesKHR(ctx_->physical_device(), surface_.get(), &count,
																		  nullptr);
	require_vk_result(result, "failed to get physical device present mode count");

	std::vector<VkPresentModeKHR> modes(count);
	result = ctx_->lib.vkGetPhysicalDeviceSurfacePresentModesKHR(ctx_->physical_device(), surface_.get(), &count, modes.data());
	require_vk_result(result, "failed to get physical device present modes");

	static constexpr VkPresentModeKHR Desired = VK_PRESENT_MODE_MAILBOX_KHR;
	if (contains(modes, Desired)) {
		present_mode_ = Desired;
	} else {
		present_mode_ = VK_PRESENT_MODE_FIFO_KHR;
	}
}

void SwapChain::init_extent_and_swapchain(RectSize size) {
	VkSurfaceCapabilitiesKHR caps;
	VkResult result = ctx_->lib.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(ctx_->physical_device(), surface_.get(), &caps);
	require_vk_result(result, "failed to get physical device surface capabilities");

	uint32_t min_image_count = caps.minImageCount + 1;
	if (caps.maxImageCount > 0 && min_image_count > caps.maxImageCount) {
		min_image_count = caps.maxImageCount;
	}

	if (caps.currentExtent.width != UINT32_MAX && caps.currentExtent.height != UINT32_MAX) {
		extent_ = caps.currentExtent;
	} else {
		extent_.width  = std::clamp(static_cast<uint32_t>(size.width), caps.minImageExtent.width, caps.maxImageExtent.width);
		extent_.height = std::clamp(static_cast<uint32_t>(size.height), caps.minImageExtent.height, caps.maxImageExtent.height);
	}

	VkBool32 supported;
	result = ctx_->lib.vkGetPhysicalDeviceSurfaceSupportKHR(ctx_->physical_device(), ctx_->presentation_queue.family,
															surface_.get(), &supported);
	require_vk_result(result, "failed to query physical device surface support");
	require(supported, "surface not supported for swap chain");

	const uint32_t queue_indices[] {ctx_->graphics_queue.family, ctx_->presentation_queue.family};
	const bool concurrent = queue_indices[0] != queue_indices[1];

	VkSwapchainCreateInfoKHR swapchain_info {
		.sType				   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext				   = nullptr,
		.flags				   = 0,
		.surface			   = surface_.get(),
		.minImageCount		   = min_image_count,
		.imageFormat		   = surface_format_.format,
		.imageColorSpace	   = surface_format_.colorSpace,
		.imageExtent		   = extent_,
		.imageArrayLayers	   = 1,
		.imageUsage			   = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode	   = concurrent ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = concurrent ? get_count(queue_indices) : 0,
		.pQueueFamilyIndices   = concurrent ? queue_indices : nullptr,
		.preTransform		   = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		.compositeAlpha		   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode		   = present_mode_,
		.clipped			   = VK_TRUE,
		.oldSwapchain		   = swapchain_, // null on first creation
	};
	result = ctx_->lib.vkCreateSwapchainKHR(ctx_->device(), &swapchain_info, nullptr, &swapchain_);
	require_vk_result(result, "failed to create Vulkan swapchain");
}

void SwapChain::init_images() {
	uint32_t count;
	VkResult result = ctx_->lib.vkGetSwapchainImagesKHR(ctx_->device(), swapchain_, &count, nullptr);
	require_vk_result(result, "failed to get Vulkan swapchain image count");

	images_.resize(count);
	result = ctx_->lib.vkGetSwapchainImagesKHR(ctx_->device(), swapchain_, &count, images_.data());
	require_vk_result(result, "failed to get Vulkan swapchain images");

	for (VkImage image : images_) {
		VkImageViewCreateInfo image_view_info {
			.sType			  = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext			  = nullptr,
			.flags			  = 0,
			.image			  = image,
			.viewType		  = VK_IMAGE_VIEW_TYPE_2D,
			.format			  = surface_format_.format,
			.components		  = VkComponentMapping {.r = VK_COMPONENT_SWIZZLE_IDENTITY,
													.g = VK_COMPONENT_SWIZZLE_IDENTITY,
													.b = VK_COMPONENT_SWIZZLE_IDENTITY,
													.a = VK_COMPONENT_SWIZZLE_IDENTITY},
			.subresourceRange = VkImageSubresourceRange {.aspectMask	 = VK_IMAGE_ASPECT_COLOR_BIT,
														 .baseMipLevel	 = 0,
														 .levelCount	 = 1,
														 .baseArrayLayer = 0,
														 .layerCount	 = 1},
		};
		VkImageView image_view;
		result = ctx_->lib.vkCreateImageView(ctx_->device(), &image_view_info, nullptr, &image_view);
		require_vk_result(result, "failed to create image view for swapchain image");
		image_views_.emplace_back(image_view);

		VkFramebufferCreateInfo framebuffer_info {
			.sType			 = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.pNext			 = nullptr,
			.flags			 = 0,
			.renderPass		 = render_pass_,
			.attachmentCount = 1,
			.pAttachments	 = &image_view,
			.width			 = extent_.width,
			.height			 = extent_.height,
			.layers			 = 1,
		};
		VkFramebuffer framebuffer;
		result = ctx_->lib.vkCreateFramebuffer(ctx_->device(), &framebuffer_info, nullptr, &framebuffer);
		require_vk_result(result, "failed to create Vulkan framebuffer for swapchain");
		framebuffers_.emplace_back(framebuffer);
	}
}

} // namespace cltv
