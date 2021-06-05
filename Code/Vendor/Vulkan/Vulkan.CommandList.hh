#pragma once

#include "Utils/Rectangle.hh"
#include "Vendor/Vulkan/Vulkan.FrameBuffer.hh"
#include "Vendor/Vulkan/Vulkan.Pipeline.hh"
#include "Vendor/Vulkan/Vulkan.RenderPass.hh"
#include "Vendor/Vulkan/Vulkan.Unique.hh"

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
		void draw();
		void end();
		void endRenderPass();

		vk::CommandBuffer handle() const
		{
			return commandList;
		}

	private:
		DeviceUnique<vk::CommandPool, &vk::Device::destroyCommandPool> commandPool;
		vk::CommandBuffer commandList;
	};
}
