#include "Queue.hpp"

#include "graphics/vulkan/DeviceContext.hpp"

namespace cltv {

Queue::Queue(const DeviceContext& ctx, uint32_t family_index) :
	_family(family_index) {
	ctx._lib.vkGetDeviceQueue(ctx._device, family_index, 0, &_queue);
}

void Queue::submit(const DeviceContext& ctx,
				   VkCommandBuffer cmd_buffer,
				   VkSemaphore wait_semaphore,
				   VkSemaphore signal_semaphore,
				   VkFence fence) {
	VkPipelineStageFlags wait_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkSubmitInfo submit_info {
		.sType				  = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount	  = 1,
		.pWaitSemaphores	  = &wait_semaphore,
		.pWaitDstStageMask	  = &wait_stages,
		.commandBufferCount	  = 1,
		.pCommandBuffers	  = &cmd_buffer,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores	  = &signal_semaphore,
	};
	// VkResult result = ctx._lib.vk
}

} // namespace cltv
