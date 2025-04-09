#pragma once

#include "graphics/vulkan/Buffer.hpp"
#include "graphics/vulkan/DeviceContext.hpp"
#include "graphics/vulkan/Pipeline.hpp"
#include "graphics/vulkan/RenderPass.hpp"
#include "graphics/vulkan/RenderTarget.hpp"

namespace cltv {

class CommandList {
public:
	explicit CommandList(const DeviceContext* ctx);

	~CommandList();
	CommandList(CommandList&&) = delete;

	void begin();
	void transition_render_target(const RenderTarget& render_target, VkImageLayout old_layout, VkImageLayout new_layout);
	void copy_buffer_to_render_target(const Buffer& buffer, const RenderTarget& render_target);
	void begin_render_pass(RectSize render_area, const RenderPass& render_pass, VkFramebuffer framebuffer);
	void bind_viewport(RectSize viewport_size);
	void bind_scissor(RectSize scissor_size);
	void bind_descriptor_set(VkPipelineLayout layout, VkDescriptorSet descriptor_set);
	void bind_pipeline(const Pipeline& pipeline);
	void draw(uint32_t vertex_count);
	void end_render_pass();
	void end();

	VkCommandBuffer get() const {
		return _cmd_buffer;
	}

private:
	const DeviceContext* _ctx;
	VkCommandPool _cmd_pool;
	VkCommandBuffer _cmd_buffer;
};

} // namespace cltv
