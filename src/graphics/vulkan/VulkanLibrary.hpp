#pragma once

#include "util/SharedLibrary.hpp"

#if CLTV_SYSTEM_WINDOWS
	#include "util/windows/WindowsDef.hpp"

	#include <vulkan/vulkan_win32.h>
#elif CLTV_SYSTEM_LINUX
	#include <vulkan/vulkan_xcb.h>
#endif

namespace cltv {

class VulkanLibrary {
public:
	VulkanLibrary();

	void load_instance_functions(VkInstance instance);
	void load_device_functions(VkDevice device);

#define macro_vk_fn(name) PFN_##name name = nullptr

	macro_vk_fn(vkGetInstanceProcAddr);
	macro_vk_fn(vkEnumerateInstanceLayerProperties);
	macro_vk_fn(vkEnumerateInstanceExtensionProperties);
	macro_vk_fn(vkCreateInstance);

	macro_vk_fn(vkDestroyInstance);
	macro_vk_fn(vkCreateDebugUtilsMessengerEXT);
	macro_vk_fn(vkDestroyDebugUtilsMessengerEXT);
	macro_vk_fn(vkEnumeratePhysicalDevices);
	macro_vk_fn(vkGetPhysicalDeviceProperties);
	macro_vk_fn(vkGetPhysicalDeviceFeatures);
	macro_vk_fn(vkGetPhysicalDeviceMemoryProperties);
	macro_vk_fn(vkGetPhysicalDeviceQueueFamilyProperties);
	macro_vk_fn(vkGetPhysicalDeviceSurfaceSupportKHR);
#if CLTV_SYSTEM_WINDOWS
	macro_vk_fn(vkCreateWin32SurfaceKHR);
#elif CLTV_SYSTEM_LINUX
	macro_vk_fn(vkCreateXcbSurfaceKHR);
#endif
	macro_vk_fn(vkDestroySurfaceKHR);
	macro_vk_fn(vkEnumerateDeviceExtensionProperties);
	macro_vk_fn(vkCreateDevice);
	macro_vk_fn(vkGetDeviceProcAddr);

	macro_vk_fn(vkDeviceWaitIdle);
	macro_vk_fn(vkDestroyDevice);
	macro_vk_fn(vkGetDeviceQueue);
	macro_vk_fn(vkCreateImage);
	macro_vk_fn(vkDestroyImage);
	macro_vk_fn(vkGetImageMemoryRequirements);
	macro_vk_fn(vkAllocateMemory);
	macro_vk_fn(vkFreeMemory);
	macro_vk_fn(vkBindImageMemory);
	macro_vk_fn(vkCreateImageView);
	macro_vk_fn(vkDestroyImageView);

#undef macro_vk_fn

private:
	SharedLibrary _lib;
};

} // namespace cltv
