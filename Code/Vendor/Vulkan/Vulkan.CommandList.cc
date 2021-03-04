#include "PCH.hh"
#include "Vulkan.CommandList.hh"

#include "Vendor/Vulkan/Vulkan.GPUContext.hh"

namespace ct::vulkan
{
	void CommandList::begin()
	{
		auto info {vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse)};
		auto result {CommandBuffer.begin(info, Loader::get())};
		ctAssertResult(result, "Failed to begin Vulkan command list.");
	}

	void CommandList::beginRenderPass(const RenderPass& renderPass, const FrameBuffer& frameBuffer)
	{
		auto info {vk::RenderPassBeginInfo().setRenderPass(renderPass.handle()).setFramebuffer(frameBuffer.handle())};
		CommandBuffer.beginRenderPass(info, vk::SubpassContents::eInline, Loader::get());
	}

	void CommandList::bindPipeline(const Pipeline& pipeline)
	{
		CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.handle(), Loader::get());
	}

	void CommandList::draw()
	{
		CommandBuffer.draw(0, 0, 0, 0, Loader::get());
	}

	void CommandList::end()
	{
		auto result {CommandBuffer.end(Loader::get())};
		ctAssertResult(result, "Failed to end Vulkan command list.");
	}

	void CommandList::endRenderPass()
	{
		CommandBuffer.endRenderPass(Loader::get());
	}
}
