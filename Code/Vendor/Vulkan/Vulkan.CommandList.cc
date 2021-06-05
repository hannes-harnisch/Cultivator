#include "PCH.hh"

#include "Vendor/Vulkan/Vulkan.GPUContext.hh"
#include "Vulkan.CommandList.hh"

namespace ct
{
	CommandList::CommandList()
	{
		auto poolInfo		 = vk::CommandPoolCreateInfo().setQueueFamilyIndex(GPUContext::graphicsQueue().getFamily());
		auto [poolRes, pool] = GPUContext::device().createCommandPool(poolInfo, nullptr, Loader::get());
		ctEnsureResult(poolRes, "Failed to create command pool.");
		commandPool = pool;

		auto bufferInfo = vk::CommandBufferAllocateInfo()
							  .setCommandPool(commandPool)
							  .setCommandBufferCount(1)
							  .setLevel(vk::CommandBufferLevel::ePrimary);
		auto [bufferRes, buffer] = GPUContext::device().allocateCommandBuffers(bufferInfo, Loader::get());
		ctEnsureResult(bufferRes, "Failed to allocate command buffer.");
		commandList = buffer[0];
	}

	void CommandList::begin()
	{
		auto info	= vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
		auto result = commandList.begin(info, Loader::get());
		ctAssertResult(result, "Failed to begin Vulkan command list.");
	}

	void CommandList::beginRenderPass(RenderPass const& renderPass, FrameBuffer const& frameBuffer)
	{
		std::array clearValues {vk::ClearValue().setColor(vk::ClearColorValue().setFloat32({1.0f, 1.0f, 0.0f, 0.0f}))};
		auto info = vk::RenderPassBeginInfo()
						.setRenderPass(renderPass.handle())
						.setFramebuffer(frameBuffer.handle())
						.setClearValues(clearValues);

		commandList.beginRenderPass(info, vk::SubpassContents::eInline, Loader::get());
	}

	void CommandList::bindViewport(Rectangle rectangle)
	{
		std::array viewports {vk::Viewport()
								  .setWidth(static_cast<float>(rectangle.width))
								  .setHeight(static_cast<float>(rectangle.height))
								  .setMaxDepth(1.0)};
		commandList.setViewport(0, viewports, Loader::get());
	}

	void CommandList::bindPipeline(Pipeline const& pipeline)
	{
		commandList.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.handle(), Loader::get());
	}

	void CommandList::draw()
	{
		commandList.draw(0, 0, 0, 0, Loader::get());
	}

	void CommandList::end()
	{
		auto result = commandList.end(Loader::get());
		ctAssertResult(result, "Failed to end Vulkan command list.");
	}

	void CommandList::endRenderPass()
	{
		commandList.endRenderPass(Loader::get());
	}

	void CommandList::reset()
	{
		GPUContext::device().resetCommandPool(commandPool, {}, Loader::get());
	}
}
