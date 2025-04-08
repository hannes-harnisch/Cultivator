#pragma once

#include "graphics/vulkan/DeviceContext.hpp"

namespace cltv {

class Buffer {
public:
	Buffer(const DeviceContext* ctx, size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	~Buffer();

	VkBuffer get_buffer() const {
		return _buffer;
	}

	VkDeviceMemory get_memory() const {
		return _memory;
	}

private:
	const DeviceContext* _ctx;
	VkBuffer _buffer;
	VkDeviceMemory _memory;
};

} // namespace cltv
