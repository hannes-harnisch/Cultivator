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
		return pipeline_;
	}

private:
	const DeviceContext* ctx_;
	VkPipeline pipeline_;
};

} // namespace cltv
