#include "PCH.hh"

#include "Vulkan.CommandList.hh"
#include "Vulkan.GPUContext.hh"

namespace ct
{
	CommandList::CommandList()
	{
		vk::CommandPoolCreateInfo poolInfo {
			.queueFamilyIndex = GPUContext::graphicsQueue().getFamily(),
		};
		auto [poolRes, pool] = GPUContext::device().createCommandPool(poolInfo);
		ctEnsureResult(poolRes, "Failed to create command pool.");
		commandPool = pool;

		vk::CommandBufferAllocateInfo bufferInfo {
			.commandPool		= commandPool,
			.commandBufferCount = 1,
		};
		auto [bufferRes, buffer] = GPUContext::device().allocateCommandBuffers(bufferInfo);
		ctEnsureResult(bufferRes, "Failed to allocate command buffer.");
		commandList = buffer[0];
	}

	void CommandList::begin()
	{
		auto res = GPUContext::device().resetCommandPool(commandPool, {});
		ctAssertResult(res, "Failed to reset command buffer.");

		vk::CommandBufferBeginInfo info;
		auto result = commandList.begin(info);
		ctAssertResult(result, "Failed to begin Vulkan command list.");
	}

	void CommandList::beginRenderPass(Rectangle const renderArea, RenderPass const& renderPass, FrameBuffer const& frameBuffer)
	{
		vk::ClearValue clear;
		clear.color = vk::ClearColorValue(std::array {1.0f, 0.0f, 1.0f, 1.0f});

		vk::RenderPassBeginInfo info {
			.renderPass		 = renderPass.handle(),
			.framebuffer	 = frameBuffer.handle(),
			.renderArea		 = vk::Rect2D({}, {renderArea.width, renderArea.height}),
			.clearValueCount = 1,
			.pClearValues	 = &clear,
		};
		commandList.beginRenderPass(info, vk::SubpassContents::eInline);
	}

	void CommandList::bindViewport(Rectangle rectangle)
	{
		vk::Viewport viewport {
			.width	  = static_cast<float>(rectangle.width),
			.height	  = static_cast<float>(rectangle.height),
			.maxDepth = 1,
		};
		commandList.setViewport(0, viewport);
	}

	void CommandList::bindScissor(Rectangle rectangle)
	{
		vk::Rect2D scissor({}, {rectangle.width, rectangle.height});
		commandList.setScissor(0, scissor);
	}

	void CommandList::bindPipeline(Pipeline const& pipeline)
	{
		commandList.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.handle());
	}

	void CommandList::bindDescriptorSets(PipelineLayout const& pipeLayout, std::vector<vk::DescriptorSet> const& sets)
	{
		commandList.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeLayout.handle(), 0, sets, {});
	}

	void CommandList::copyBufferToTexture(Buffer const& src, Texture const& dst)
	{
		vk::ImageSubresourceLayers subresourceLayers {
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.layerCount = 1,
		};
		vk::BufferImageCopy region {
			.imageSubresource = subresourceLayers,
			.imageExtent	  = vk::Extent3D(dst.size().width, dst.size().height, 1),
		};
		commandList.copyBufferToImage(src.buffer(), dst.image(), vk::ImageLayout::eTransferDstOptimal, region);
	}

	void CommandList::transitionTexture(Texture const& tex, vk::ImageLayout newLayout)
	{
		vk::ImageSubresourceRange subresourceRange {
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.levelCount = 1,
			.layerCount = 1,
		};
		vk::ImageMemoryBarrier barrier {
			.oldLayout		  = vk::ImageLayout::eUndefined,
			.newLayout		  = newLayout,
			.image			  = tex.image(),
			.subresourceRange = subresourceRange,
		};

		vk::PipelineStageFlagBits srcStage, dstStage;
		if(newLayout == vk::ImageLayout::eTransferDstOptimal)
		{
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
			srcStage			  = vk::PipelineStageFlagBits::eTopOfPipe;
			dstStage			  = vk::PipelineStageFlagBits::eTransfer;
		}
		else if(newLayout == vk::ImageLayout::eColorAttachmentOptimal)
		{
			barrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
			srcStage			  = vk::PipelineStageFlagBits::eFragmentShader;
			dstStage			  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		}
		else if(newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
		{
			barrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
			srcStage			  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
			dstStage			  = vk::PipelineStageFlagBits::eFragmentShader;
		}
		else
			throw "Unsupported layout transition!";

		commandList.pipelineBarrier(srcStage, dstStage, {}, {}, {}, barrier);
	}

	void CommandList::draw()
	{
		commandList.draw(3, 1, 0, 0);
	}

	void CommandList::end()
	{
		auto res = commandList.end();
		ctAssertResult(res, "Failed to end Vulkan command list.");
	}

	void CommandList::endRenderPass()
	{
		commandList.endRenderPass();
	}
}
