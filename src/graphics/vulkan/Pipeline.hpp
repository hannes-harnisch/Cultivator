#pragma once

#include "graphics/vulkan/DeviceContext.hpp"
#include "graphics/vulkan/RenderPass.hpp"

namespace cltv {

class Pipeline {
public:
	Pipeline(const DeviceContext* ctx,
			 VkShaderModule vertex,
			 VkShaderModule fragment,
			 VkPipelineLayout layout,
			 const RenderPass& render_pass);

	~Pipeline();

	VkPipeline get() const {
		return _pipeline;
	}

private:
	const DeviceContext* _ctx;
	VkPipeline _pipeline;
};

} // namespace cltv
