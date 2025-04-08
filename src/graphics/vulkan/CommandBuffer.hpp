#pragma once

#include "graphics/vulkan/DeviceContext.hpp"

namespace cltv {

class CommandBuffer {
public:
	explicit CommandBuffer(const DeviceContext* ctx);
	~CommandBuffer();

	void begin();
	void end();

private:
	const DeviceContext* _ctx;
	VkCommandPool _cmd_pool;
	VkCommandBuffer _cmd_buffer;
};

} // namespace cltv
