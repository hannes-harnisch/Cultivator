#include "RenderTarget.hpp"

#include "graphics/vulkan/Util.hpp"
#include "util/Util.hpp"

namespace cltv {

RenderTarget::RenderTarget(const DeviceContext* ctx, RectSize size, const RenderPass& render_pass) :
	_ctx(ctx),
	_size(size) {
	init_image(size);
	init_memory();
	init_view_and_framebuffer(size, render_pass);
}

RenderTarget::~RenderTarget() {
	_ctx->lib.vkDestroyFramebuffer(_ctx->device(), _framebuffer, nullptr);
	_ctx->lib.vkDestroyImageView(_ctx->device(), _image_view, nullptr);
	_ctx->lib.vkDestroyImage(_ctx->device(), _image, nullptr);
	_ctx->lib.vkFreeMemory(_ctx->device(), _memory, nullptr);
}

void RenderTarget::init_image(RectSize size) {
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
	VkResult result = _ctx->lib.vkCreateImage(_ctx->device(), &image_info, nullptr, &_image);
	require_vk_result(result, "failed to create Vulkan image");
}

void RenderTarget::init_memory() {
	VkMemoryRequirements requirements;
	_ctx->lib.vkGetImageMemoryRequirements(_ctx->device(), _image, &requirements);

	auto mem_type_index = _ctx->find_memory_type_index(requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	require(mem_type_index.has_value(), "no suitable memory type for texture");

	static constexpr VkDeviceSize MinRecommended = 1048576;

	VkMemoryAllocateInfo alloc_info {
		.sType			 = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext			 = nullptr,
		.allocationSize	 = std::max(requirements.size, MinRecommended),
		.memoryTypeIndex = *mem_type_index,
	};
	VkResult result = _ctx->lib.vkAllocateMemory(_ctx->device(), &alloc_info, nullptr, &_memory);
	require_vk_result(result, "failed to allocate GPU memory for texture");

	result = _ctx->lib.vkBindImageMemory(_ctx->device(), _image, _memory, 0);
	require_vk_result(result, "failed to bind image memory");
}

void RenderTarget::init_view_and_framebuffer(RectSize size, const RenderPass& render_pass) {
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
	VkResult result = _ctx->lib.vkCreateImageView(_ctx->device(), &image_view_info, nullptr, &_image_view);
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
	result = _ctx->lib.vkCreateFramebuffer(_ctx->device(), &framebuffer_info, nullptr, &_framebuffer);
	require_vk_result(result, "failed to create Vulkan render target framebuffer");
}

} // namespace cltv
