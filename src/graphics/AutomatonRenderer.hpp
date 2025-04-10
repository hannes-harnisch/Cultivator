#pragma once

#include "app/Window.hpp"
#include "graphics/vulkan/CommandList.hpp"
#include "graphics/vulkan/DeviceContext.hpp"
#include "graphics/vulkan/Pipeline.hpp"
#include "graphics/vulkan/RenderPass.hpp"
#include "graphics/vulkan/RenderTarget.hpp"
#include "graphics/vulkan/SwapChain.hpp"
#include "util/RectSize.hpp"

namespace cltv {

struct RendererParams {
	RectSize universe_size;
	const char* simulation_shader_path;
	uint32_t initial_live_cell_incidence;
	uint64_t delay_milliseconds;
};

class AutomatonRenderer {
public:
	AutomatonRenderer(const DeviceContext* ctx, const Window* window, const RendererParams& params);

	~AutomatonRenderer();

	void draw_frame();

private:
	static constexpr uint32_t MaxFrames = 2;

	const DeviceContext* _ctx;
	uint64_t _delay_milliseconds;
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
	VkDescriptorPool _descriptor_pool;
	VkDescriptorSet _descriptor_set_front;
	VkDescriptorSet _descriptor_set_back;
	VkFence _frame_fences[MaxFrames];
	VkSemaphore _img_release_semaphores[MaxFrames];
	VkSemaphore _img_acquire_semaphores[MaxFrames];
	std::vector<VkFence> _image_pending_fences;
	uint32_t _current_frame = 0;
	bool _first_frame_done	= false;
	std::deque<CommandList> _cmd_lists;

	void record_commands(uint32_t image_index);
	void prepare_render_targets(uint32_t initial_live_cell_incidence);

	VkShaderModule create_shader_module(const char* path) const;
	VkDescriptorSetLayout create_descriptor_set_layout() const;
	VkPipelineLayout create_pipeline_layout() const;
	VkSampler create_sampler() const;
	VkDescriptorPool create_descriptor_pool() const;
	VkDescriptorSet create_descriptor_set(const RenderTarget& render_target) const;
	VkFence create_fence() const;
	VkSemaphore create_semaphore() const;
};

} // namespace cltv
