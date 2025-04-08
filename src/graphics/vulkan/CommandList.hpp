#pragma once

#include "graphics/vulkan/Buffer.hpp"
#include "graphics/vulkan/DeviceContext.hpp"
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
