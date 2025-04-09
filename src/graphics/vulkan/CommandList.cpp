#include "CommandList.hpp"

#include "graphics/vulkan/Util.hpp"
#include "util/Util.hpp"

namespace cltv {

CommandList::CommandList(const DeviceContext* ctx) :
	_ctx(ctx) {
	VkCommandPoolCreateInfo pool_info {
		.sType			  = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext			  = nullptr,
		.flags			  = 0,
		.queueFamilyIndex = ctx->graphics_queue.family,
	};
	VkResult result = ctx->lib.vkCreateCommandPool(ctx->device(), &pool_info, nullptr, &_cmd_pool);
	require_vk_result(result, "failed to create command pool");

	VkCommandBufferAllocateInfo alloc_info {
		.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext				= nullptr,
		.commandPool		= _cmd_pool,
		.level				= VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1,
	};
	result = ctx->lib.vkAllocateCommandBuffers(ctx->device(), &alloc_info, &_cmd_buffer);
	require_vk_result(result, "failed to allocate command buffer");
}

CommandList::~CommandList() {
	_ctx->lib.vkDestroyCommandPool(_ctx->device(), _cmd_pool, nullptr);
}

void CommandList::begin() {
	VkResult result = _ctx->lib.vkResetCommandPool(_ctx->device(), _cmd_pool, 0);
	require_vk_result(result, "failed to reset command pool");

	VkCommandBufferBeginInfo begin_info {
		.sType			  = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext			  = nullptr,
		.flags			  = 0,
		.pInheritanceInfo = nullptr,
	};
	result = _ctx->lib.vkBeginCommandBuffer(_cmd_buffer, &begin_info);
	require_vk_result(result, "failed to begin command buffer");
}

void CommandList::transition_render_target(const RenderTarget& render_target,
										   VkImageLayout old_layout,
										   VkImageLayout new_layout) {
	VkPipelineStageFlags src_stage;
	VkAccessFlags src_access;
	switch (old_layout) {
	case VK_IMAGE_LAYOUT_UNDEFINED:
		src_access = 0;
		src_stage  = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		src_access = VK_ACCESS_TRANSFER_WRITE_BIT;
		src_stage  = VK_PIPELINE_STAGE_TRANSFER_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		src_access = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		src_stage  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		src_access = VK_ACCESS_SHADER_READ_BIT;
		src_stage  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		break;

	default:
		throw std::runtime_error("invalid old layout");
	}

	VkPipelineStageFlags dst_stage;
	VkAccessFlags dst_access;
	switch (new_layout) {
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		dst_access = VK_ACCESS_TRANSFER_WRITE_BIT;
		dst_stage  = VK_PIPELINE_STAGE_TRANSFER_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		dst_access = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dst_stage  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		dst_access = VK_ACCESS_SHADER_READ_BIT;
		dst_stage  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		break;

	default:
		throw std::runtime_error("invalid new layout");
	}

	VkImageMemoryBarrier barrier {
		.sType				 = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.pNext				 = nullptr,
		.srcAccessMask		 = src_access,
		.dstAccessMask		 = dst_access,
		.oldLayout			 = old_layout,
		.newLayout			 = new_layout,
		.srcQueueFamilyIndex = 0,
		.dstQueueFamilyIndex = 0,
		.image				 = render_target.get_image(),
		.subresourceRange	 = VkImageSubresourceRange {.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT,
														.baseMipLevel	= 0,
														.levelCount		= 1,
														.baseArrayLayer = 0,
														.layerCount		= 1},
	};
	_ctx->lib.vkCmdPipelineBarrier(_cmd_buffer, src_stage, dst_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void CommandList::copy_buffer_to_render_target(const Buffer& buffer, const RenderTarget& render_target) {
	RectSize size = render_target.get_size();
	VkBufferImageCopy region {
		.bufferOffset	   = 0,
		.bufferRowLength   = 0,
		.bufferImageHeight = 0,
		.imageSubresource  = VkImageSubresourceLayers {.aspectMask	   = VK_IMAGE_ASPECT_COLOR_BIT,
													   .mipLevel	   = 0,
													   .baseArrayLayer = 0,
													   .layerCount	   = 1},
		.imageOffset	   = VkOffset3D {.x = 0, .y = 0, .z = 0},
		.imageExtent	   = VkExtent3D {.width	 = static_cast<uint32_t>(size.width),
										 .height = static_cast<uint32_t>(size.height),
										 .depth	 = 1},
	};
	_ctx->lib.vkCmdCopyBufferToImage(_cmd_buffer, buffer.get_buffer(), render_target.get_image(),
									 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

void CommandList::begin_render_pass(RectSize render_area, const RenderPass& render_pass, VkFramebuffer framebuffer) {
	VkRenderPassBeginInfo begin_info {
		.sType			 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext			 = nullptr,
		.renderPass		 = render_pass.get(),
		.framebuffer	 = framebuffer,
		.renderArea		 = VkRect2D {.offset = VkOffset2D {.x = 0, .y = 0},
									 .extent = VkExtent2D {.width  = static_cast<uint32_t>(render_area.width),
														   .height = static_cast<uint32_t>(render_area.height)}},
		.clearValueCount = 0,
		.pClearValues	 = nullptr,
	};
	_ctx->lib.vkCmdBeginRenderPass(_cmd_buffer, &begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandList::bind_viewport(RectSize viewport_size) {
	VkViewport viewport {
		.x		  = 0,
		.y		  = 0,
		.width	  = static_cast<float>(viewport_size.width),
		.height	  = static_cast<float>(viewport_size.height),
		.minDepth = 0,
		.maxDepth = 1,
	};
	_ctx->lib.vkCmdSetViewport(_cmd_buffer, 0, 1, &viewport);
}

void CommandList::bind_scissor(RectSize scissor_size) {
	VkRect2D scissor {
		.offset = VkOffset2D {.x = 0, .y = 0},
		.extent = VkExtent2D {.width  = static_cast<uint32_t>(scissor_size.width),
							  .height = static_cast<uint32_t>(scissor_size.height)},
	};
	_ctx->lib.vkCmdSetScissor(_cmd_buffer, 0, 1, &scissor);
}

void CommandList::bind_descriptor_set(VkPipelineLayout layout, VkDescriptorSet descriptor_set) {
	_ctx->lib.vkCmdBindDescriptorSets(_cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &descriptor_set, 0, nullptr);
}

void CommandList::bind_pipeline(const Pipeline& pipeline) {
	_ctx->lib.vkCmdBindPipeline(_cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.get());
}

void CommandList::draw(uint32_t vertex_count) {
	_ctx->lib.vkCmdDraw(_cmd_buffer, vertex_count, 1, 0, 0);
}

void CommandList::end_render_pass() {
	_ctx->lib.vkCmdEndRenderPass(_cmd_buffer);
}

void CommandList::end() {
	VkResult result = _ctx->lib.vkEndCommandBuffer(_cmd_buffer);
	require_vk_result(result, "failed to end command buffer");
}

} // namespace cltv
