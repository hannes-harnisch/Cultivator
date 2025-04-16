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

	const DeviceContext* ctx_;
	uint64_t delay_milliseconds_;
	RenderPass simulation_pass_;
	RenderPass presentation_pass_;
	SwapChain swap_chain_;
	RenderTarget front_target_;
	RenderTarget back_target_;
	VkShaderModule vertex_shader_;
	VkShaderModule simulation_fragment_shader_;
	VkShaderModule presentation_fragment_shader_;
	VkDescriptorSetLayout descriptor_set_layout_;
	VkPipelineLayout pipeline_layout_;
	Pipeline simulation_pipeline_;
	Pipeline presentation_pipeline_;
	VkSampler sampler_;
	VkDescriptorPool descriptor_pool_;
	VkDescriptorSet descriptor_set_front_;
	VkDescriptorSet descriptor_set_back_;
	VkFence frame_fences_[MaxFrames];
	VkSemaphore img_release_semaphores_[MaxFrames];
	VkSemaphore img_acquire_semaphores_[MaxFrames];
	std::vector<VkFence> image_pending_fences_;
	uint32_t current_frame_ = 0;
	bool first_frame_done_	= false;
	std::deque<CommandList> cmd_lists_;

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
