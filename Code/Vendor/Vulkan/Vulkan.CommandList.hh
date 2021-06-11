#pragma once

#include "Utils/Rectangle.hh"
#include "Vulkan.Buffer.hh"
#include "Vulkan.FrameBuffer.hh"
#include "Vulkan.Pipeline.hh"
#include "Vulkan.PipelineLayout.hh"
#include "Vulkan.RenderPass.hh"
#include "Vulkan.Texture.hh"
#include "Vulkan.Unique.hh"

namespace ct
{
	class CommandList
	{
	public:
		CommandList();

		void begin();
		void beginRenderPass(Rectangle renderArea, RenderPass const& renderPass, FrameBuffer const& frameBuffer);
		void bindViewport(Rectangle rectangle);
		void bindScissor(Rectangle rectangle);
		void bindPipeline(Pipeline const& pipeline);
		void bindDescriptorSets(PipelineLayout const& pipeLayout, std::vector<vk::DescriptorSet> const& sets);
		void copyBufferToTexture(Buffer const& src, Texture const& dst);
		void pushImageBarrier(Texture const& tex, vk::ImageLayout newLayout);
		void draw();
		void end();
		void endRenderPass();

		vk::CommandBuffer handle() const
		{
			return commandList;
		}

	private:
		DeviceOwn<vk::CommandPool, &vk::Device::destroyCommandPool> commandPool;
		vk::CommandBuffer commandList;
	};
}
