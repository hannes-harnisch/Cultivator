#include "PCH.hh"

#include "Utils/Assert.hh"
#include "Vulkan.GPUContext.hh"
#include "Vulkan.Queue.hh"
#include "Vulkan.Surface.hh"

namespace ct
{
	Queue::Queue(uint32_t familyIndex) : familyIndex(familyIndex), queue(GPUContext::device().getQueue(familyIndex, 0))
	{}

	bool Queue::supportsSurface(Surface const& surface) const
	{
		auto handle			 = surface.handle();
		auto [res, supports] = GPUContext::adapter().getSurfaceSupportKHR(familyIndex, handle);
		ctEnsureResult(res, "Failed to query for Vulkan surface support.");
		return supports;
	}

	void Queue::submit(vk::CommandBuffer commandBuffer, vk::Semaphore wait, vk::Semaphore signal, vk::Fence fence)
	{
		vk::PipelineStageFlags waitStages = vk::PipelineStageFlagBits::eColorAttachmentOutput;

		vk::SubmitInfo submit {
			.waitSemaphoreCount	  = 1,
			.pWaitSemaphores	  = &wait,
			.pWaitDstStageMask	  = &waitStages,
			.commandBufferCount	  = 1,
			.pCommandBuffers	  = &commandBuffer,
			.signalSemaphoreCount = 1,
			.pSignalSemaphores	  = &signal,
		};
		ctAssertResult(queue.submit(submit, fence), "Failed to submit to queue.");
	}

	void Queue::submitSync(vk::CommandBuffer commandBuffer)
	{
		vk::SubmitInfo submit {
			.commandBufferCount = 1,
			.pCommandBuffers	= &commandBuffer,
		};
		ctAssertResult(queue.submit(submit, nullptr), "Failed to submit to queue.");
		ctAssertResult(queue.waitIdle(), "Failed to wait for queue idle.");
	}
}
