#include "DeviceContext.hpp"

#include "graphics/vulkan/Surface.hpp"
#include "graphics/vulkan/Util.hpp"
#include "util/Util.hpp"

namespace cltv {

constexpr inline const char* RequiredLayers[] = {
	"VK_LAYER_KHRONOS_validation",
};

constexpr inline const char* RequiredInstanceExtensions[] = {
	"VK_KHR_surface",

#if CLTV_SYSTEM_WINDOWS
	"VK_KHR_win32_surface",
#elif CLTV_SYSTEM_LINUX
	"VK_KHR_xcb_surface",
#endif

	"VK_EXT_debug_utils", // will be excluded if debug layer not enabled
};

constexpr inline VkValidationFeatureEnableEXT ValidationFeatures[] = {
	VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
	VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT,
};

constexpr inline VkBool32 VkPhysicalDeviceFeatures::* RequiredDeviceFeatures[] = {
	&VkPhysicalDeviceFeatures::samplerAnisotropy,
	&VkPhysicalDeviceFeatures::shaderImageGatherExtended,
};

constexpr inline const char* RequiredDeviceExtensions[] = {
	"VK_KHR_swapchain",
};

static VKAPI_ATTR VkBool32 VKAPI_PTR debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT,
													VkDebugUtilsMessageTypeFlagsEXT,
													const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
													void*) {
	if (pCallbackData->messageIdNumber != 0x675dc32e) { // ignore enabling VK_EXT_debug_utils
		std::cerr << pCallbackData->pMessage;
	}
	return false;
}

DeviceContext::DeviceContext(Window& window, bool enable_debug_layer) :
	_lib() {
	if (enable_debug_layer) {
		check_layers();
	}
	check_instance_extensions();
	init_instance(enable_debug_layer);
	init_physical_device();
	init_queue_families(window);
	check_device_extensions();
	init_device(enable_debug_layer);
}

DeviceContext::~DeviceContext() {
	VkResult result = _lib.vkDeviceWaitIdle(_device);
	require_vk_result(result, "failed to wait for device idle state");

	_lib.vkDestroyDevice(_device, nullptr);
	if (_messenger != VK_NULL_HANDLE) {
		_lib.vkDestroyDebugUtilsMessengerEXT(_instance, _messenger, nullptr);
	}
	_lib.vkDestroyInstance(_instance, nullptr);
}

void DeviceContext::submit_to_queue(Queue queue,
									VkCommandBuffer cmd_buffer,
									VkSemaphore wait_semaphore,
									VkSemaphore signal_semaphore,
									VkFence fence) const {
	VkPipelineStageFlags wait_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkSubmitInfo submit_info {
		.sType				  = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext				  = nullptr,
		.waitSemaphoreCount	  = 1,
		.pWaitSemaphores	  = &wait_semaphore,
		.pWaitDstStageMask	  = &wait_stages,
		.commandBufferCount	  = 1,
		.pCommandBuffers	  = &cmd_buffer,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores	  = &signal_semaphore,
	};
	VkResult result = _lib.vkQueueSubmit(queue.queue, 1, &submit_info, fence);
	require_vk_result(result, "failed to submit to queue");
}

void DeviceContext::submit_to_queue_blocking(Queue queue, VkCommandBuffer cmd_buffer) const {
	VkSubmitInfo submit_info {
		.sType				  = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext				  = nullptr,
		.waitSemaphoreCount	  = 0,
		.pWaitSemaphores	  = nullptr,
		.pWaitDstStageMask	  = nullptr,
		.commandBufferCount	  = 1,
		.pCommandBuffers	  = &cmd_buffer,
		.signalSemaphoreCount = 0,
		.pSignalSemaphores	  = nullptr,
	};
	VkResult result = _lib.vkQueueSubmit(queue.queue, 1, &submit_info, nullptr);
	require_vk_result(result, "failed to submit to queue blocking");

	result = _lib.vkQueueWaitIdle(queue.queue);
	require_vk_result(result, "failed to wait for queue idle");
}

std::optional<uint32_t> DeviceContext::find_memory_type_index(uint32_t supported_type_mask,
															  VkMemoryPropertyFlags desired_properties) const {
	for (uint32_t i = 0; i < _mem_properties.memoryTypeCount; ++i) {
		const bool current_type_supported = has_flags(supported_type_mask, 1u << i);
		if (current_type_supported && has_flags(_mem_properties.memoryTypes[i].propertyFlags, desired_properties)) {
			return i;
		}
	}
	return std::nullopt;
}

void DeviceContext::check_layers() const {
	uint32_t count;
	VkResult result = _lib.vkEnumerateInstanceLayerProperties(&count, nullptr);
	require_vk_result(result, "failed to query Vulkan layer count");

	std::vector<VkLayerProperties> layers(count);
	result = _lib.vkEnumerateInstanceLayerProperties(&count, layers.data());
	require_vk_result(result, "failed to query Vulkan layers");

	for (std::string_view required_layer : RequiredLayers) {
		bool found = contains(layers, required_layer, &VkLayerProperties::layerName);
		require(found, "failed to find required Vulkan layer");
	}
}

void DeviceContext::check_instance_extensions() const {
	uint32_t count;
	VkResult result = _lib.vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
	require_vk_result(result, "failed to query Vulkan instance extension count");

	std::vector<VkExtensionProperties> extensions(count);
	result = _lib.vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());
	require_vk_result(result, "failed to query Vulkan instance extensions");

	for (std::string_view required_extension : RequiredInstanceExtensions) {
		bool found = contains(extensions, required_extension, &VkExtensionProperties::extensionName);
		require(found, "failed to find required Vulkan instance extension");
	}
}

void DeviceContext::init_instance(bool enable_debug_layer) {
	VkDebugUtilsMessengerCreateInfoEXT messenger_info {
		.sType			 = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.pNext			 = nullptr,
		.flags			 = 0,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		.messageType	 = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
					   | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		.pfnUserCallback = debug_callback,
		.pUserData		 = nullptr,
	};
	VkValidationFeaturesEXT validation_features {
		.sType							= VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
		.pNext							= &messenger_info,
		.enabledValidationFeatureCount	= get_count(ValidationFeatures),
		.pEnabledValidationFeatures		= ValidationFeatures,
		.disabledValidationFeatureCount = 0,
		.pDisabledValidationFeatures	= nullptr,
	};
	VkApplicationInfo application_info {
		.sType				= VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext				= nullptr,
		.pApplicationName	= "Cultivator",
		.applicationVersion = VK_MAKE_VERSION(0, 0, 0),
		.pEngineName		= "Cultivator",
		.engineVersion		= VK_MAKE_VERSION(0, 0, 0),
		.apiVersion			= VK_API_VERSION_1_0,
	};
	VkInstanceCreateInfo instance_info {
		.sType					 = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext					 = enable_debug_layer ? &validation_features : nullptr,
		.flags					 = 0,
		.pApplicationInfo		 = &application_info,
		.enabledLayerCount		 = enable_debug_layer ? get_count(RequiredLayers) : 0,
		.ppEnabledLayerNames	 = RequiredLayers,
		.enabledExtensionCount	 = enable_debug_layer ? get_count(RequiredInstanceExtensions)
													  : get_count(RequiredInstanceExtensions) - 1,
		.ppEnabledExtensionNames = RequiredInstanceExtensions,
	};
	VkResult result = _lib.vkCreateInstance(&instance_info, nullptr, &_instance);
	require_vk_result(result, "failed to create Vulkan instance");

	_lib.load_instance_functions(_instance);

	if (enable_debug_layer) {
		result = _lib.vkCreateDebugUtilsMessengerEXT(_instance, &messenger_info, nullptr, &_messenger);
		require_vk_result(result, "failed to create Vulkan debug messenger");
	}
}

void DeviceContext::init_physical_device() {
	uint32_t count;
	VkResult result = _lib.vkEnumeratePhysicalDevices(_instance, &count, nullptr);
	require_vk_result(result, "failed to query Vulkan physical device count");

	std::vector<VkPhysicalDevice> physical_devices(count);
	result = _lib.vkEnumeratePhysicalDevices(_instance, &count, physical_devices.data());
	require_vk_result(result, "failed to query Vulkan physical devices");

	for (auto physical_device : physical_devices) {
		VkPhysicalDeviceProperties properties;
		_lib.vkGetPhysicalDeviceProperties(physical_device, &properties);

		VkPhysicalDeviceFeatures features;
		_lib.vkGetPhysicalDeviceFeatures(physical_device, &features);

		auto has_features = [&]() -> bool {
			return std::ranges::all_of(RequiredDeviceFeatures, [&](VkBool32 VkPhysicalDeviceFeatures::* feature) -> bool {
				return features.*feature == VK_TRUE;
			});
		};

		if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU && has_features()) {
			_physical_device = physical_device;
		}

		if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && has_features()) {
			_physical_device = physical_device;
			break;
		}
	}
	require(_physical_device != VK_NULL_HANDLE, "no suitable physical device found");

	_lib.vkGetPhysicalDeviceMemoryProperties(_physical_device, &_mem_properties);
}

void DeviceContext::init_queue_families(Window& window) {
	uint32_t count;
	_lib.vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &count, nullptr);

	std::vector<VkQueueFamilyProperties> queue_family_properties(count);
	_lib.vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &count, queue_family_properties.data());

	Surface surface(*this, window);
	uint32_t index = 0;
	for (auto& queue_family : queue_family_properties) {
		if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			_graphics_queue.family = index;
		}

		VkBool32 supported;
		VkResult result = _lib.vkGetPhysicalDeviceSurfaceSupportKHR(_physical_device, index, surface.get(), &supported);
		require_vk_result(result, "failed to query Vulkan surface support");
		if (supported) {
			_presentation_queue.family = index;
		}
		++index;
	}
	surface.destroy(*this);

	require(_graphics_queue.family != UINT32_MAX, "Vulkan driver does not support graphics queues");
	require(_presentation_queue.family != UINT32_MAX, "Vulkan driver does not support presentation queues");
}

void DeviceContext::check_device_extensions() const {
	uint32_t count;
	VkResult result = _lib.vkEnumerateDeviceExtensionProperties(_physical_device, nullptr, &count, nullptr);
	require_vk_result(result, "failed to query Vulkan physical device count");

	std::vector<VkExtensionProperties> extensions(count);
	result = _lib.vkEnumerateDeviceExtensionProperties(_physical_device, nullptr, &count, extensions.data());
	require_vk_result(result, "failed to query Vulkan physical devices");

	for (std::string_view required_extension : RequiredDeviceExtensions) {
		bool found = contains(extensions, required_extension, &VkExtensionProperties::extensionName);
		require(found, "failed to find required Vulkan device extension");
	}
}

void DeviceContext::init_device(bool enable_debug_layer) {
	const float queue_priorities[] = {1.0f};

	const VkDeviceQueueCreateInfo queue_infos[] = {
		VkDeviceQueueCreateInfo {
			.sType			  = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.pNext			  = nullptr,
			.flags			  = 0,
			.queueFamilyIndex = _graphics_queue.family,
			.queueCount		  = 1,
			.pQueuePriorities = queue_priorities,
		},
		VkDeviceQueueCreateInfo {
			.sType			  = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.pNext			  = nullptr,
			.flags			  = 0,
			.queueFamilyIndex = _presentation_queue.family,
			.queueCount		  = 1,
			.pQueuePriorities = queue_priorities,
		},
	};

	VkPhysicalDeviceFeatures enabled_features {};
	for (auto feature : RequiredDeviceFeatures) {
		enabled_features.*feature = VK_TRUE;
	}

	VkDeviceCreateInfo device_info {
		.sType					 = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext					 = nullptr,
		.flags					 = 0,
		.queueCreateInfoCount	 = _graphics_queue.family != _presentation_queue.family ? 2u : 1u,
		.pQueueCreateInfos		 = queue_infos,
		.enabledLayerCount		 = enable_debug_layer ? get_count(RequiredLayers) : 0,
		.ppEnabledLayerNames	 = RequiredLayers,
		.enabledExtensionCount	 = get_count(RequiredDeviceExtensions),
		.ppEnabledExtensionNames = RequiredDeviceExtensions,
		.pEnabledFeatures		 = &enabled_features,
	};
	VkResult result = _lib.vkCreateDevice(_physical_device, &device_info, nullptr, &_device);
	require_vk_result(result, "failed to create Vulkan device");

	_lib.load_device_functions(_device);

	_lib.vkGetDeviceQueue(_device, _graphics_queue.family, 0, &_graphics_queue.queue);
	_lib.vkGetDeviceQueue(_device, _presentation_queue.family, 0, &_presentation_queue.queue);
}

} // namespace cltv
