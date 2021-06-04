#pragma once

#include "Vendor/Vulkan/Vulkan.FrameBuffer.hh"
#include "Vendor/Vulkan/Vulkan.Pipeline.hh"
#include "Vendor/Vulkan/Vulkan.RenderPass.hh"
#include "Vendor/Vulkan/Vulkan.Unique.hh"

namespace ct
{
	class CommandList final
	{
	public:
		void begin();
		void beginRenderPass(RenderPass const& renderPass, FrameBuffer const& frameBuffer);
		void bindPipeline(Pipeline const& pipeline);
		void draw();
		void end();
		void endRenderPass();

	private:
		vk::CommandBuffer commandList;
	};
}
