#include "SwapChain.hpp"

#include "graphics/vulkan/Util.hpp"
#include "util/Util.hpp"

namespace cltv {

SwapChain::SwapChain(const DeviceContext* ctx, RectSize size, Window& window, const RenderPass& render_pass) :
	_ctx(ctx),
	_surface(ctx, window) {
	init_surface_format();
	init_present_mode();
	init_extent_and_swapchain(size);
	init_images(render_pass);
}

SwapChain::~SwapChain() {
	for (VkFramebuffer framebuffer : _framebuffers) {
		_ctx->lib.vkDestroyFramebuffer(_ctx->device(), framebuffer, nullptr);
	}
	for (VkImageView image_view : _image_views) {
		_ctx->lib.vkDestroyImageView(_ctx->device(), image_view, nullptr);
	}
	// swapchain owns the VkImages, we don't need to destroy them

	_ctx->lib.vkDestroySwapchainKHR(_ctx->device(), _swapchain, nullptr);
}

uint32_t SwapChain::get_next_image_index(VkSemaphore img_acquire_semaphore) {
	uint32_t index;
	VkResult result = _ctx->lib.vkAcquireNextImageKHR(_ctx->device(), _swapchain, UINT64_MAX, img_acquire_semaphore,
													  VK_NULL_HANDLE, &index);
	require_vk_result(result, "could not acquire next swapchain image");
	return index;
}

void SwapChain::present(uint32_t image_index, VkSemaphore img_release_semaphore) {
	VkPresentInfoKHR present_info {
		.sType				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext				= nullptr,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores	= &img_release_semaphore,
		.swapchainCount		= 1,
		.pSwapchains		= &_swapchain,
		.pImageIndices		= &image_index,
		.pResults			= nullptr,
	};
	VkResult result = _ctx->lib.vkQueuePresentKHR(_ctx->presentation_queue.queue, &present_info);
	require_vk_result(result, "could not present swapchain image to queue");
}

void SwapChain::init_surface_format() {
	uint32_t count;
	VkResult result = _ctx->lib.vkGetPhysicalDeviceSurfaceFormatsKHR(_ctx->physical_device(), _surface.get(), &count, nullptr);
	require_vk_result(result, "failed to get physical device surface format count");

	std::vector<VkSurfaceFormatKHR> formats(count);
	result = _ctx->lib.vkGetPhysicalDeviceSurfaceFormatsKHR(_ctx->physical_device(), _surface.get(), &count, formats.data());
	require_vk_result(result, "failed to get physical device surface formats");

	static constexpr VkSurfaceFormatKHR Desired {
		.format		= VK_FORMAT_B8G8R8A8_SRGB,
		.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
	};
	auto predicate = [](VkSurfaceFormatKHR format) {
		return format.format == Desired.format && format.colorSpace == Desired.colorSpace;
	};
	if (std::ranges::find_if(formats, predicate) != formats.end()) {
		_surface_format = Desired;
	} else {
		_surface_format = formats.at(0);
	}
}

void SwapChain::init_present_mode() {
	uint32_t count;
	VkResult result = _ctx->lib.vkGetPhysicalDeviceSurfacePresentModesKHR(_ctx->physical_device(), _surface.get(), &count,
																		  nullptr);
	require_vk_result(result, "failed to get physical device present mode count");

	std::vector<VkPresentModeKHR> modes(count);
	result = _ctx->lib.vkGetPhysicalDeviceSurfacePresentModesKHR(_ctx->physical_device(), _surface.get(), &count, modes.data());
	require_vk_result(result, "failed to get physical device present modes");

	static constexpr VkPresentModeKHR Desired = VK_PRESENT_MODE_MAILBOX_KHR;
	if (contains(modes, Desired)) {
		_present_mode = Desired;
	} else {
		_present_mode = VK_PRESENT_MODE_FIFO_KHR;
	}
}

void SwapChain::init_extent_and_swapchain(RectSize size) {
	VkSurfaceCapabilitiesKHR caps;
	VkResult result = _ctx->lib.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_ctx->physical_device(), _surface.get(), &caps);
	require_vk_result(result, "failed to get physical device surface capabilities");

	uint32_t min_image_count = caps.minImageCount + 1;
	if (caps.maxImageCount > 0 && min_image_count > caps.maxImageCount) {
		min_image_count = caps.maxImageCount;
	}

	if (caps.currentExtent.width != UINT32_MAX && caps.currentExtent.height != UINT32_MAX) {
		_extent = caps.currentExtent;
	} else {
		_extent.width  = std::clamp(static_cast<uint32_t>(size.width), caps.minImageExtent.width, caps.maxImageExtent.width);
		_extent.height = std::clamp(static_cast<uint32_t>(size.height), caps.minImageExtent.height, caps.maxImageExtent.height);
	}

	VkBool32 supported;
	result = _ctx->lib.vkGetPhysicalDeviceSurfaceSupportKHR(_ctx->physical_device(), _ctx->presentation_queue.family,
															_surface.get(), &supported);
	require_vk_result(result, "failed to query physical device surface support");
	require(supported, "surface not supported for swap chain");

	const uint32_t queue_indices[] {_ctx->graphics_queue.family, _ctx->presentation_queue.family};
	const bool concurrent = queue_indices[0] != queue_indices[1];

	VkSwapchainCreateInfoKHR swapchain_info {
		.sType				   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext				   = nullptr,
		.flags				   = 0,
		.surface			   = _surface.get(),
		.minImageCount		   = min_image_count,
		.imageFormat		   = _surface_format.format,
		.imageColorSpace	   = _surface_format.colorSpace,
		.imageExtent		   = _extent,
		.imageArrayLayers	   = 1,
		.imageUsage			   = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode	   = concurrent ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = concurrent ? get_count(queue_indices) : 0,
		.pQueueFamilyIndices   = concurrent ? queue_indices : nullptr,
		.preTransform		   = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		.compositeAlpha		   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode		   = _present_mode,
		.clipped			   = VK_TRUE,
		.oldSwapchain		   = VK_NULL_HANDLE,
	};
	result = _ctx->lib.vkCreateSwapchainKHR(_ctx->device(), &swapchain_info, nullptr, &_swapchain);
	require_vk_result(result, "failed to create Vulkan swapchain");
}

void SwapChain::init_images(const RenderPass& render_pass) {
	uint32_t count;
	VkResult result = _ctx->lib.vkGetSwapchainImagesKHR(_ctx->device(), _swapchain, &count, nullptr);
	require_vk_result(result, "failed to get Vulkan swapchain image count");

	_images.resize(count);
	result = _ctx->lib.vkGetSwapchainImagesKHR(_ctx->device(), _swapchain, &count, _images.data());
	require_vk_result(result, "failed to get Vulkan swapchain images");

	for (VkImage image : _images) {
		VkImageViewCreateInfo image_view_info {
			.sType			  = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext			  = nullptr,
			.flags			  = 0,
			.image			  = image,
			.viewType		  = VK_IMAGE_VIEW_TYPE_2D,
			.format			  = _surface_format.format,
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
		result = _ctx->lib.vkCreateImageView(_ctx->device(), &image_view_info, nullptr, &image_view);
		require_vk_result(result, "failed to create image view for swapchain image");
		_image_views.emplace_back(image_view);

		VkFramebufferCreateInfo framebuffer_info {
			.sType			 = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.pNext			 = nullptr,
			.flags			 = 0,
			.renderPass		 = render_pass.get(),
			.attachmentCount = 1,
			.pAttachments	 = &image_view,
			.width			 = static_cast<uint32_t>(_extent.width),
			.height			 = static_cast<uint32_t>(_extent.height),
			.layers			 = 1,
		};
		VkFramebuffer framebuffer;
		result = _ctx->lib.vkCreateFramebuffer(_ctx->device(), &framebuffer_info, nullptr, &framebuffer);
		require_vk_result(result, "failed to create Vulkan framebuffer for swapchain");
		_framebuffers.emplace_back(framebuffer);
	}
}

} // namespace cltv
