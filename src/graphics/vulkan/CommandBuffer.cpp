#include "CommandBuffer.hpp"

#include "graphics/vulkan/Util.hpp"

namespace cltv {

CommandBuffer::CommandBuffer(const DeviceContext* ctx) :
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

CommandBuffer::~CommandBuffer() {
	_ctx->lib.vkDestroyCommandPool(_ctx->device(), _cmd_pool, nullptr);
}

void CommandBuffer::begin() {
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

void CommandBuffer::end() {
	VkResult result = _ctx->lib.vkEndCommandBuffer(_cmd_buffer);
	require_vk_result(result, "failed to end command buffer");
}

} // namespace cltv
