#include "PCH.hh"

#include "Vendor/Vulkan/Vulkan.GPUContext.hh"
#include "Vulkan.CommandList.hh"

namespace ct::vulkan
{
	void CommandList::begin()
	{
		auto info	= vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
		auto result = commandList.begin(info, Loader::get());
		ctAssertResult(result, "Failed to begin Vulkan command list.");
	}

	void CommandList::beginRenderPass(RenderPass const& renderPass, FrameBuffer const& frameBuffer)
	{
		auto info = vk::RenderPassBeginInfo().setRenderPass(renderPass.handle()).setFramebuffer(frameBuffer.handle());
		commandList.beginRenderPass(info, vk::SubpassContents::eInline, Loader::get());
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
}
