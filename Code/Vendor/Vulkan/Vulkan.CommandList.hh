#pragma once

#include "Utils/Rectangle.hh"
#include "Vulkan.FrameBuffer.hh"
#include "Vulkan.Pipeline.hh"
#include "Vulkan.PipelineLayout.hh"
#include "Vulkan.RenderPass.hh"
#include "Vulkan.Unique.hh"

namespace ct
{
	class CommandList
	{
	public:
		CommandList();

		void begin();
		void beginRenderPass(RenderPass const& renderPass, FrameBuffer const& frameBuffer);
		void bindViewport(Rectangle rectangle);
		void bindPipeline(Pipeline const& pipeline);
		void bindDescriptorSets(PipelineLayout const& pipeLayout, std::vector<vk::DescriptorSet> const& sets);
		void draw();
		void end();
		void endRenderPass();
		void reset();

		vk::CommandBuffer handle() const
		{
			return commandList;
		}

	private:
		DeviceOwn<vk::CommandPool, &vk::Device::destroyCommandPool> commandPool;
		vk::CommandBuffer commandList;
	};
}
