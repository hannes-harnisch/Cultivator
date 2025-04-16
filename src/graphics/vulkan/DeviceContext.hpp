#pragma once

#include "app/Window.hpp"
#include "graphics/vulkan/VulkanLibrary.hpp"

namespace cltv {

class DeviceContext {
public:
	struct Queue {
		uint32_t family = UINT32_MAX;
		VkQueue queue	= VK_NULL_HANDLE;
	};

	VulkanLibrary lib;
	Queue graphics_queue;
	Queue presentation_queue;

	DeviceContext(const Window& window, bool enable_debug_layer);

	~DeviceContext();
	DeviceContext(DeviceContext&&) = delete;

	void submit_to_queue(Queue queue,
						 VkCommandBuffer cmd_buffer,
						 VkSemaphore wait_semaphore,
						 VkSemaphore signal_semaphore,
						 VkFence fence) const;

	void submit_to_queue_blocking(Queue queue, VkCommandBuffer cmd_buffer) const;

	std::optional<uint32_t> find_memory_type_index(uint32_t supported_type_mask,
												   VkMemoryPropertyFlags desired_properties) const;

	VkInstance instance() const {
		return instance_;
	}

	VkPhysicalDevice physical_device() const {
		return physical_device_;
	}

	VkDevice device() const {
		return device_;
	}

private:
	VkInstance instance_							 = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT messenger_				 = VK_NULL_HANDLE;
	VkPhysicalDevice physical_device_				 = VK_NULL_HANDLE;
	VkDevice device_								 = VK_NULL_HANDLE;
	VkPhysicalDeviceMemoryProperties mem_properties_ = {};

	void check_layers() const;
	void check_instance_extensions() const;
	void init_instance(bool enable_debug_layer);
	void init_physical_device();
	void init_queue_families(const Window& window);
	void check_device_extensions() const;
	void init_device(bool enable_debug_layer);
};

} // namespace cltv
