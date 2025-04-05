#pragma once

#include "app/Window.hpp"
#include "graphics/vulkan/Queue.hpp"
#include "graphics/vulkan/VulkanLibrary.hpp"

namespace cltv {

struct DeviceContext {
	VulkanLibrary _lib;
	Queue _graphics_queue;
	Queue _presentation_queue;
	VkInstance _instance							 = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT _messenger				 = VK_NULL_HANDLE;
	VkPhysicalDevice _physical_device				 = VK_NULL_HANDLE;
	VkDevice _device								 = VK_NULL_HANDLE;
	VkPhysicalDeviceMemoryProperties _mem_properties = {};

	DeviceContext(Window& window, bool enable_debug_layer);

	~DeviceContext();
	DeviceContext(DeviceContext&&) = delete;

	std::optional<uint32_t> find_memory_type_index(uint32_t supported_type_mask,
												   VkMemoryPropertyFlags desired_properties) const;
};

} // namespace cltv
