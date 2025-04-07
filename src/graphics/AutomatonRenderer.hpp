#pragma once

#include "app/Window.hpp"
#include "graphics/vulkan/DeviceContext.hpp"
#include "graphics/vulkan/Pipeline.hpp"
#include "graphics/vulkan/RenderPass.hpp"
#include "graphics/vulkan/RenderTarget.hpp"
#include "graphics/vulkan/SwapChain.hpp"
#include "util/RectSize.hpp"

namespace cltv {

class AutomatonRenderer {
public:
	AutomatonRenderer(const DeviceContext* ctx, Window& window, RectSize size, const char* shader_path);
	~AutomatonRenderer();

	void draw_frame();

private:
	const DeviceContext* _ctx;
	RectSize _window_size;
	RenderPass _simulation_pass;
	RenderPass _presentation_pass;
	SwapChain _swap_chain;
	RenderTarget _front_target;
	RenderTarget _back_target;
	VkShaderModule _vertex_shader;
	VkShaderModule _simulation_fragment_shader;
	VkShaderModule _presentation_fragment_shader;
	VkDescriptorSetLayout _descriptor_set_layout;
	VkPipelineLayout _pipeline_layout;
	Pipeline _simulation_pipeline;
	Pipeline _presentation_pipeline;
	VkSampler _sampler;

	VkShaderModule create_shader_module(const char* path) const;
	VkDescriptorSetLayout create_descriptor_set_layout() const;
	VkPipelineLayout create_pipeline_layout() const;
	VkSampler create_sampler() const;
};

} // namespace cltv
