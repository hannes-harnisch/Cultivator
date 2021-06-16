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

		vk::SubmitInfo submit;
		submit.waitSemaphoreCount	= 1;
		submit.pWaitSemaphores		= &wait;
		submit.pWaitDstStageMask	= &waitStages;
		submit.commandBufferCount	= 1;
		submit.pCommandBuffers		= &commandBuffer;
		submit.signalSemaphoreCount = 1;
		submit.pSignalSemaphores	= &signal;

		ctAssertResult(queue.submit(submit, fence), "Failed to submit to queue.");
	}

	void Queue::submitSync(vk::CommandBuffer commandBuffer)
	{
		vk::SubmitInfo submit;
		submit.commandBufferCount = 1;
		submit.pCommandBuffers	  = &commandBuffer;

		ctAssertResult(queue.submit(submit, nullptr), "Failed to submit to queue.");
		ctAssertResult(queue.waitIdle(), "Failed to wait for queue idle.");
	}
}
