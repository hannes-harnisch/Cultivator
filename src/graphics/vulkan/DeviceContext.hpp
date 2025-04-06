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

	VulkanLibrary _lib;
	VkInstance _instance			  = VK_NULL_HANDLE;
	VkPhysicalDevice _physical_device = VK_NULL_HANDLE;
	VkDevice _device				  = VK_NULL_HANDLE;
	Queue _graphics_queue;
	Queue _presentation_queue;

	DeviceContext(Window& window, bool enable_debug_layer);

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

private:
	VkDebugUtilsMessengerEXT _messenger				 = VK_NULL_HANDLE;
	VkPhysicalDeviceMemoryProperties _mem_properties = {};

	void check_layers() const;
	void check_instance_extensions() const;
	void init_instance(bool enable_debug_layer);
	void init_physical_device();
	void init_queue_families(Window& window);
	void check_device_extensions() const;
	void init_device(bool enable_debug_layer);
};

} // namespace cltv
