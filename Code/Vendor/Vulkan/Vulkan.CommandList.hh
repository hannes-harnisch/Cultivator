#pragma once

#include "Vendor/Vulkan/Vulkan.FrameBuffer.hh"
#include "Vendor/Vulkan/Vulkan.Pipeline.hh"
#include "Vendor/Vulkan/Vulkan.RenderPass.hh"

namespace ct::vulkan
{
	class CommandList final
	{
	public:
		void begin();
		void beginRenderPass(const RenderPass& renderPass, const FrameBuffer& frameBuffer);
		void bindPipeline(const Pipeline& pipeline);
		void draw();
		void end();
		void endRenderPass();

	private:
		vk::CommandBuffer CommandBuffer;
	};
}
