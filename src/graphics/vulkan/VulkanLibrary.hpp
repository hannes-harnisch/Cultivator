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

	macro_vk_fn(vkCreateDebugUtilsMessengerEXT);
	macro_vk_fn(vkEnumeratePhysicalDevices);
	macro_vk_fn(vkGetPhysicalDeviceProperties);
	macro_vk_fn(vkGetPhysicalDeviceFeatures);
	macro_vk_fn(vkGetPhysicalDeviceMemoryProperties);
	macro_vk_fn(vkGetPhysicalDeviceQueueFamilyProperties);
	macro_vk_fn(vkGetPhysicalDeviceSurfaceSupportKHR);
	macro_vk_fn(vkGetPhysicalDeviceSurfaceFormatsKHR);
	macro_vk_fn(vkGetPhysicalDeviceSurfacePresentModesKHR);
	macro_vk_fn(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
#if CLTV_SYSTEM_WINDOWS
	macro_vk_fn(vkCreateWin32SurfaceKHR);
#elif CLTV_SYSTEM_LINUX
	macro_vk_fn(vkCreateXcbSurfaceKHR);
#endif
	macro_vk_fn(vkEnumerateDeviceExtensionProperties);
	macro_vk_fn(vkCreateDevice);
	macro_vk_fn(vkGetDeviceProcAddr);
	macro_vk_fn(vkDestroySurfaceKHR);
	macro_vk_fn(vkDestroyDebugUtilsMessengerEXT);
	macro_vk_fn(vkDestroyInstance);

	macro_vk_fn(vkGetDeviceQueue);
	macro_vk_fn(vkCreateRenderPass);
	macro_vk_fn(vkCreateSwapchainKHR);
	macro_vk_fn(vkCreateImageView);
	macro_vk_fn(vkCreateFramebuffer);
	macro_vk_fn(vkCreateImage);
	macro_vk_fn(vkCreateBuffer);
	macro_vk_fn(vkCreateShaderModule);
	macro_vk_fn(vkCreateDescriptorSetLayout);
	macro_vk_fn(vkCreatePipelineLayout);
	macro_vk_fn(vkCreateGraphicsPipelines);
	macro_vk_fn(vkCreateSampler);
	macro_vk_fn(vkCreateDescriptorPool);
	macro_vk_fn(vkCreateFence);
	macro_vk_fn(vkCreateSemaphore);
	macro_vk_fn(vkCreateCommandPool);
	macro_vk_fn(vkGetSwapchainImagesKHR);
	macro_vk_fn(vkGetImageMemoryRequirements);
	macro_vk_fn(vkGetBufferMemoryRequirements);
	macro_vk_fn(vkAllocateMemory);
	macro_vk_fn(vkFreeMemory);
	macro_vk_fn(vkBindImageMemory);
	macro_vk_fn(vkBindBufferMemory);
	macro_vk_fn(vkMapMemory);
	macro_vk_fn(vkUnmapMemory);
	macro_vk_fn(vkAllocateDescriptorSets);
	macro_vk_fn(vkUpdateDescriptorSets);
	macro_vk_fn(vkAllocateCommandBuffers);
	macro_vk_fn(vkResetCommandPool);
	macro_vk_fn(vkBeginCommandBuffer);
	macro_vk_fn(vkCmdPipelineBarrier);
	macro_vk_fn(vkCmdCopyBufferToImage);
	macro_vk_fn(vkCmdBeginRenderPass);
	macro_vk_fn(vkCmdSetViewport);
	macro_vk_fn(vkCmdSetScissor);
	macro_vk_fn(vkCmdBindPipeline);
	macro_vk_fn(vkCmdBindDescriptorSets);
	macro_vk_fn(vkCmdDraw);
	macro_vk_fn(vkCmdEndRenderPass);
	macro_vk_fn(vkEndCommandBuffer);
	macro_vk_fn(vkWaitForFences);
	macro_vk_fn(vkResetFences);
	macro_vk_fn(vkAcquireNextImageKHR);
	macro_vk_fn(vkQueuePresentKHR);
	macro_vk_fn(vkQueueSubmit);
	macro_vk_fn(vkQueueWaitIdle);
	macro_vk_fn(vkDeviceWaitIdle);
	macro_vk_fn(vkDestroyCommandPool);
	macro_vk_fn(vkDestroySemaphore);
	macro_vk_fn(vkDestroyFence);
	macro_vk_fn(vkDestroyDescriptorPool);
	macro_vk_fn(vkDestroySampler);
	macro_vk_fn(vkDestroyPipeline);
	macro_vk_fn(vkDestroyPipelineLayout);
	macro_vk_fn(vkDestroyDescriptorSetLayout);
	macro_vk_fn(vkDestroyShaderModule);
	macro_vk_fn(vkDestroyBuffer);
	macro_vk_fn(vkDestroyImage);
	macro_vk_fn(vkDestroyFramebuffer);
	macro_vk_fn(vkDestroyImageView);
	macro_vk_fn(vkDestroySwapchainKHR);
	macro_vk_fn(vkDestroyRenderPass);
	macro_vk_fn(vkDestroyDevice);

#undef macro_vk_fn

private:
	SharedLibrary lib;
};

} // namespace cltv
