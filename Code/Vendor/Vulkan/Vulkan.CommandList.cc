#include "PCH.hh"

#include "Vulkan.CommandList.hh"
#include "Vulkan.GPUContext.hh"

namespace ct
{
	CommandList::CommandList()
	{
		auto poolInfo = vk::CommandPoolCreateInfo()
							.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
							.setQueueFamilyIndex(GPUContext::graphicsQueue().getFamily());
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
		vk::CommandBufferBeginInfo info;
		auto result = commandList.begin(info, Loader::get());
		ctAssertResult(result, "Failed to begin Vulkan command list.");
	}

	void CommandList::beginRenderPass(Rectangle const renderArea, RenderPass const& renderPass, FrameBuffer const& frameBuffer)
	{
		std::array clearValues {vk::ClearValue().setColor(vk::ClearColorValue().setFloat32({1.0f, 1.0f, 0.0f, 0.0f}))};
		auto info = vk::RenderPassBeginInfo()
						.setRenderPass(renderPass.handle())
						.setFramebuffer(frameBuffer.handle())
						.setRenderArea({{0, 0}, {renderArea.width, renderArea.height}})
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

	void CommandList::bindScissor(Rectangle rectangle)
	{
		std::array scissors {vk::Rect2D().setExtent({rectangle.width, rectangle.height})};
		commandList.setScissor(0, scissors, Loader::get());
	}

	void CommandList::bindPipeline(Pipeline const& pipeline)
	{
		commandList.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.handle(), Loader::get());
	}

	void CommandList::bindDescriptorSets(PipelineLayout const& pipeLayout, std::vector<vk::DescriptorSet> const& sets)
	{
		commandList.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeLayout.handle(), 0, sets, {}, Loader::get());
	}

	void CommandList::copyBufferToTexture(Buffer const& src, Texture const& dst)
	{
		auto subresourceLayers = vk::ImageSubresourceLayers().setAspectMask(vk::ImageAspectFlagBits::eColor).setLayerCount(1);
		std::array regions {vk::BufferImageCopy()
								.setImageSubresource(subresourceLayers)
								.setImageExtent({dst.size().width, dst.size().height, 1})};
		commandList.copyBufferToImage(src.buffer(), dst.image(), vk::ImageLayout::eTransferDstOptimal, regions, Loader::get());
	}

	void CommandList::pushImageBarrier(Texture const& tex, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
	{
		auto subresourceRange =
			vk::ImageSubresourceRange().setAspectMask(vk::ImageAspectFlagBits::eColor).setLevelCount(1).setLayerCount(1);
		auto barrier = vk::ImageMemoryBarrier()
						   .setOldLayout(oldLayout)
						   .setNewLayout(newLayout)
						   .setImage(tex.image())
						   .setSubresourceRange(subresourceRange);

		vk::PipelineStageFlagBits srcStage, dstStage;
		if(oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
		{
			barrier.srcAccessMask = {};
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
			srcStage			  = vk::PipelineStageFlagBits::eTopOfPipe;
			dstStage			  = vk::PipelineStageFlagBits::eTransfer;
		}
		else if(oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eColorAttachmentOptimal)
		{
			barrier.srcAccessMask = {};
			barrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
			srcStage			  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
			dstStage			  = vk::PipelineStageFlagBits::eFragmentShader;
		}
		else if(oldLayout == vk::ImageLayout::eShaderReadOnlyOptimal && newLayout == vk::ImageLayout::eColorAttachmentOptimal)
		{
			barrier.srcAccessMask = {};
			barrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
			srcStage			  = vk::PipelineStageFlagBits::eFragmentShader;
			dstStage			  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		}
		else if(oldLayout == vk::ImageLayout::eColorAttachmentOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
		{
			barrier.srcAccessMask = {};
			barrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
			srcStage			  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
			dstStage			  = vk::PipelineStageFlagBits::eFragmentShader;
		}
		else if(oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
		{
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
			srcStage			  = vk::PipelineStageFlagBits::eTransfer;
			dstStage			  = vk::PipelineStageFlagBits::eFragmentShader;
		}
		else
			throw "Unsupported layout transition!";

		std::array imgBarriers {barrier};
		commandList.pipelineBarrier(srcStage, dstStage, {}, {}, {}, imgBarriers, Loader::get());
	}

	void CommandList::draw()
	{
		commandList.draw(3, 1, 0, 0, Loader::get());
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
