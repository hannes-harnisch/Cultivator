#include "RenderTarget.hpp"

#include "graphics/vulkan/Util.hpp"
#include "util/Util.hpp"

namespace cltv {

RenderTarget::RenderTarget(const DeviceContext& ctx, RectSize size, const RenderPass& render_pass) :
	_size(size) {
	init_image(ctx, size);
	init_memory(ctx);
	init_view_and_framebuffer(ctx, size, render_pass);
}

RenderTarget::~RenderTarget() {
	assert(_memory == VK_NULL_HANDLE);
	assert(_image == VK_NULL_HANDLE);
	assert(_image_view == VK_NULL_HANDLE);
	assert(_framebuffer == VK_NULL_HANDLE);
}

void RenderTarget::destroy(const DeviceContext& ctx) {
	ctx._lib.vkDestroyFramebuffer(ctx._device, _framebuffer, nullptr);
	_framebuffer = VK_NULL_HANDLE;

	ctx._lib.vkDestroyImageView(ctx._device, _image_view, nullptr);
	_image_view = VK_NULL_HANDLE;

	ctx._lib.vkDestroyImage(ctx._device, _image, nullptr);
	_image = VK_NULL_HANDLE;

	ctx._lib.vkFreeMemory(ctx._device, _memory, nullptr);
	_memory = VK_NULL_HANDLE;
}

void RenderTarget::init_image(const DeviceContext& ctx, RectSize size) {
	VkImageCreateInfo image_info {
		.sType		 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext		 = nullptr,
		.flags		 = 0,
		.imageType	 = VK_IMAGE_TYPE_2D,
		.format		 = VK_FORMAT_B8G8R8A8_SRGB,
		.extent		 = VkExtent3D {.width  = static_cast<uint32_t>(size.width),
								   .height = static_cast<uint32_t>(size.height),
								   .depth  = 1},
		.mipLevels	 = 1,
		.arrayLayers = 1,
		.samples	 = VK_SAMPLE_COUNT_1_BIT,
		.tiling		 = VK_IMAGE_TILING_OPTIMAL,
		.usage		 = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices   = nullptr,
		.initialLayout		   = VK_IMAGE_LAYOUT_UNDEFINED,
	};
	VkResult result = ctx._lib.vkCreateImage(ctx._device, &image_info, nullptr, &_image);
	require_vk_result(result, "failed to create Vulkan image");
}

void RenderTarget::init_memory(const DeviceContext& ctx) {
	VkMemoryRequirements requirements;
	ctx._lib.vkGetImageMemoryRequirements(ctx._device, _image, &requirements);

	auto mem_type_index = ctx.find_memory_type_index(requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	require(mem_type_index.has_value(), "no suitable memory type for texture");

	static constexpr VkDeviceSize MinRecommended = 1048576;

	VkMemoryAllocateInfo alloc_info {
		.sType			 = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext			 = nullptr,
		.allocationSize	 = std::max(requirements.size, MinRecommended),
		.memoryTypeIndex = *mem_type_index,
	};
	VkResult result = ctx._lib.vkAllocateMemory(ctx._device, &alloc_info, nullptr, &_memory);
	require_vk_result(result, "failed to allocate GPU memory for texture");

	result = ctx._lib.vkBindImageMemory(ctx._device, _image, _memory, 0);
	require_vk_result(result, "failed to bind image memory");
}

void RenderTarget::init_view_and_framebuffer(const DeviceContext& ctx, RectSize size, const RenderPass& render_pass) {
	VkImageViewCreateInfo image_view_info {
		.sType			  = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext			  = nullptr,
		.flags			  = 0,
		.image			  = _image,
		.viewType		  = VK_IMAGE_VIEW_TYPE_2D,
		.format			  = VK_FORMAT_B8G8R8A8_SRGB,
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
	VkResult result = ctx._lib.vkCreateImageView(ctx._device, &image_view_info, nullptr, &_image_view);
	require_vk_result(result, "failed to create Vulkan image view");

	VkFramebufferCreateInfo framebuffer_info {
		.sType			 = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.pNext			 = nullptr,
		.flags			 = 0,
		.renderPass		 = render_pass.get(),
		.attachmentCount = 1,
		.pAttachments	 = &_image_view,
		.width			 = static_cast<uint32_t>(size.width),
		.height			 = static_cast<uint32_t>(size.height),
		.layers			 = 1,
	};
	result = ctx._lib.vkCreateFramebuffer(ctx._device, &framebuffer_info, nullptr, &_framebuffer);
	require_vk_result(result, "failed to create Vulkan render target framebuffer");
}

} // namespace cltv
