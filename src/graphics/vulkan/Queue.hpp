#pragma once

namespace cltv {

struct DeviceContext;

class Queue {
public:
	Queue() = default;

	Queue(const DeviceContext& ctx, uint32_t family_index);

	void submit(const DeviceContext& ctx,
				VkCommandBuffer cmd_buffer,
				VkSemaphore wait_semaphore,
				VkSemaphore signal_semaphore,
				VkFence fence);

	uint32_t get_family() const {
		return _family;
	}

	VkQueue get() const {
		return _queue;
	}

private:
	uint32_t _family = UINT32_MAX;
	VkQueue _queue	 = VK_NULL_HANDLE;
};

} // namespace cltv
