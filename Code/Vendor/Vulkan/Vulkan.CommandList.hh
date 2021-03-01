#pragma once

#include "Vendor/Vulkan/Vulkan.Pipeline.hh"
#include "Vendor/Vulkan/Vulkan.RenderTarget.hh"

namespace ct::vulkan
{
	class CommandList final
	{
	public:
		void begin();
		void beginRenderPass(const RenderTarget& renderTarget);
		void bindPipeline(const Pipeline& pipeline);
		void draw();
		void end();
		void endRenderPass();

	private:
		vk::CommandBuffer CommandBuffer;
	};
}
