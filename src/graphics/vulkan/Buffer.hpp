#pragma once

#include "graphics/vulkan/DeviceContext.hpp"

namespace cltv {

class Buffer {
public:
	Buffer(const DeviceContext* ctx, size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	~Buffer();

	VkBuffer get_buffer() const {
		return buffer_;
	}

	VkDeviceMemory get_memory() const {
		return memory_;
	}

private:
	const DeviceContext* ctx_;
	VkBuffer buffer_;
	VkDeviceMemory memory_;
};

} // namespace cltv
