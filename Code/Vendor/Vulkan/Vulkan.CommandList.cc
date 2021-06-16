#include "PCH.hh"

#include "Vulkan.CommandList.hh"
#include "Vulkan.GPUContext.hh"

namespace ct
{
	CommandList::CommandList()
	{
		vk::CommandPoolCreateInfo poolInfo;
		poolInfo.queueFamilyIndex = GPUContext::graphicsQueue().getFamily();

		auto [poolRes, pool] = GPUContext::device().createCommandPool(poolInfo, nullptr, Loader::get());
		ctEnsureResult(poolRes, "Failed to create command pool.");
		commandPool = pool;
	}

	void CommandList::begin()
	{
		vk::CommandBufferAllocateInfo bufferInfo;
		bufferInfo.commandPool		  = commandPool;
		bufferInfo.level			  = vk::CommandBufferLevel::ePrimary;
		bufferInfo.commandBufferCount = 1;

		auto [bufferRes, buffer] = GPUContext::device().allocateCommandBuffers(bufferInfo, Loader::get());
		ctEnsureResult(bufferRes, "Failed to allocate command buffer.");
		commandList = buffer[0];

		vk::CommandBufferBeginInfo info;
		auto result = commandList.begin(info, Loader::get());
		ctAssertResult(result, "Failed to begin Vulkan command list.");
	}

	void CommandList::beginRenderPass(Rectangle const renderArea, RenderPass const& renderPass, FrameBuffer const& frameBuffer)
	{
		vk::ClearValue clear;
		clear.color = vk::ClearColorValue(std::array {1.0f, 0.0f, 1.0f, 1.0f});

		vk::RenderPassBeginInfo info;
		info.renderPass		 = renderPass.handle();
		info.framebuffer	 = frameBuffer.handle();
		info.renderArea		 = vk::Rect2D({}, {renderArea.width, renderArea.height});
		info.clearValueCount = 1;
		info.pClearValues	 = &clear;

		commandList.beginRenderPass(info, vk::SubpassContents::eInline, Loader::get());
	}

	void CommandList::bindViewport(Rectangle rectangle)
	{
		vk::Viewport viewport;
		viewport.width	  = static_cast<float>(rectangle.width);
		viewport.height	  = static_cast<float>(rectangle.height);
		viewport.maxDepth = 1;
		commandList.setViewport(0, viewport, Loader::get());
	}

	void CommandList::bindScissor(Rectangle rectangle)
	{
		vk::Rect2D scissor({}, {rectangle.width, rectangle.height});
		commandList.setScissor(0, scissor, Loader::get());
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
		vk::ImageSubresourceLayers subresourceLayers;
		subresourceLayers.aspectMask = vk::ImageAspectFlagBits::eColor;
		subresourceLayers.layerCount = 1;

		vk::BufferImageCopy region;
		region.imageSubresource = subresourceLayers;
		region.imageExtent		= vk::Extent3D(dst.size().width, dst.size().height, 1);

		commandList.copyBufferToImage(src.buffer(), dst.image(), vk::ImageLayout::eTransferDstOptimal, region, Loader::get());
	}

	void CommandList::transitionTexture(Texture const& tex, vk::ImageLayout newLayout)
	{
		vk::ImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		subresourceRange.levelCount = 1;
		subresourceRange.layerCount = 1;

		vk::ImageMemoryBarrier barrier;
		barrier.oldLayout		 = vk::ImageLayout::eUndefined;
		barrier.newLayout		 = newLayout;
		barrier.image			 = tex.image();
		barrier.subresourceRange = subresourceRange;

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

		commandList.pipelineBarrier(srcStage, dstStage, {}, {}, {}, barrier, Loader::get());
	}

	void CommandList::draw()
	{
		commandList.draw(3, 1, 0, 0, Loader::get());
	}

	void CommandList::end()
	{
		auto res = commandList.end(Loader::get());
		ctAssertResult(res, "Failed to end Vulkan command list.");
	}

	void CommandList::endRenderPass()
	{
		commandList.endRenderPass(Loader::get());
	}

	void CommandList::reset()
	{
		GPUContext::device().freeCommandBuffers(commandPool, commandList, Loader::get());
		auto res = GPUContext::device().resetCommandPool(commandPool, {}, Loader::get());
		ctAssertResult(res, "Failed to reset command buffer.");
	}
}
