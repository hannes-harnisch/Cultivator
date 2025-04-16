#include "RenderTarget.hpp"

#include "graphics/vulkan/Util.hpp"
#include "util/Util.hpp"

namespace cltv {

RenderTarget::RenderTarget(const DeviceContext* ctx, RectSize size, const RenderPass& render_pass) :
	ctx_(ctx),
	size_(size) {
	init_image(size);
	init_memory();
	init_view_and_framebuffer(size, render_pass);
}

RenderTarget::~RenderTarget() {
	ctx_->lib.vkDestroyFramebuffer(ctx_->device(), framebuffer_, nullptr);
	ctx_->lib.vkDestroyImageView(ctx_->device(), image_view_, nullptr);
	ctx_->lib.vkDestroyImage(ctx_->device(), image_, nullptr);
	ctx_->lib.vkFreeMemory(ctx_->device(), memory_, nullptr);
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
	VkResult result = ctx_->lib.vkCreateImage(ctx_->device(), &image_info, nullptr, &image_);
	require_vk_result(result, "failed to create Vulkan image");
}

void RenderTarget::init_memory() {
	VkMemoryRequirements requirements;
	ctx_->lib.vkGetImageMemoryRequirements(ctx_->device(), image_, &requirements);

	auto mem_type_index = ctx_->find_memory_type_index(requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	require(mem_type_index.has_value(), "no suitable memory type for texture");

	static constexpr VkDeviceSize MinRecommended = 1048576;

	VkMemoryAllocateInfo alloc_info {
		.sType			 = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext			 = nullptr,
		.allocationSize	 = std::max(requirements.size, MinRecommended),
		.memoryTypeIndex = *mem_type_index,
	};
	VkResult result = ctx_->lib.vkAllocateMemory(ctx_->device(), &alloc_info, nullptr, &memory_);
	require_vk_result(result, "failed to allocate GPU memory for texture");

	result = ctx_->lib.vkBindImageMemory(ctx_->device(), image_, memory_, 0);
	require_vk_result(result, "failed to bind image memory");
}

void RenderTarget::init_view_and_framebuffer(RectSize size, const RenderPass& render_pass) {
	VkImageViewCreateInfo image_view_info {
		.sType			  = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext			  = nullptr,
		.flags			  = 0,
		.image			  = image_,
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
	VkResult result = ctx_->lib.vkCreateImageView(ctx_->device(), &image_view_info, nullptr, &image_view_);
	require_vk_result(result, "failed to create Vulkan image view");

	VkFramebufferCreateInfo framebuffer_info {
		.sType			 = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.pNext			 = nullptr,
		.flags			 = 0,
		.renderPass		 = render_pass.get(),
		.attachmentCount = 1,
		.pAttachments	 = &image_view_,
		.width			 = static_cast<uint32_t>(size.width),
		.height			 = static_cast<uint32_t>(size.height),
		.layers			 = 1,
	};
	result = ctx_->lib.vkCreateFramebuffer(ctx_->device(), &framebuffer_info, nullptr, &framebuffer_);
	require_vk_result(result, "failed to create Vulkan render target framebuffer");
}

} // namespace cltv
