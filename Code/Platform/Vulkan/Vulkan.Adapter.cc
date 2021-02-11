#include "Platform/Vulkan/Vulkan.Adapter.hh"

#include "Assert.hh"
#include <cstdio>
#include <vector>

namespace ct::vulkan
{
	namespace
	{
		VkApplicationInfo getAppInfo()
		{
			return {.sType				= VK_STRUCTURE_TYPE_APPLICATION_INFO,
					.pApplicationName	= CT_APP_NAME,
					.applicationVersion = VK_MAKE_VERSION(0, 0, 1),
					.pEngineName		= CT_APP_NAME,
					.engineVersion		= VK_MAKE_VERSION(0, 0, 1),
					.apiVersion			= VK_API_VERSION_1_0};
		}

		VKAPI_ATTR VkBool32 VKAPI_CALL logDebug(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
												VkDebugUtilsMessageTypeFlagsEXT messageTypes,
												const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
												void* pUserData)
		{
			if(messageSeverity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
				return false;

			std::string msgType;
			switch(messageTypes)
			{
				case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: msgType = "GENERAL"; break;
				case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: msgType = "VALIDATION"; break;
				case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: msgType = "PERFORMANCE"; break;
			}
			std::printf("\n[DEBUG] [%s] %s\n", msgType.data(), pCallbackData->pMessage);
			return false;
		}

		VkDebugUtilsMessengerCreateInfoEXT getLoggerInfo()
		{
			return {.sType			 = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
					.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
									   VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
									   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
					.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
								   VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
								   VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
					.pfnUserCallback = logDebug};
		}
	}

	Adapter::Adapter()
	{
		ensureInstanceExtensionsExist();
#if CT_DEBUG
		ensureDebugLayersExist();
#endif
		initializeInstance();
	}

	Adapter::Adapter(Adapter&& other) noexcept
	{
		*this = std::move(other);
	}

	Adapter::~Adapter()
	{
#if CT_DEBUG
		auto func {vkGetInstanceProcAddr(Instance, "vkDestroyDebugUtilsMessengerEXT")};
		auto vkDestroyDebugUtilsMessengerEXT {reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(func)};
		vkDestroyDebugUtilsMessengerEXT(Instance, Logger, nullptr);
#endif

		vkDestroyInstance(Instance, nullptr);
	}

	Adapter& Adapter::operator=(Adapter&& other) noexcept
	{
		std::swap(Instance, other.Instance);
		std::swap(Logger, other.Logger);
		return *this;
	}

	void Adapter::ensureInstanceExtensionsExist()
	{
		uint32_t extensionCount {0};
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		for(auto requiredExtension : RequiredInstanceExtensions)
		{
			bool found = false;
			for(auto& extension : extensions)
				if(std::strcmp(extension.extensionName, requiredExtension) == 0)
				{
					found = true;
					break;
				}
			ctEnsure(found, "Failed to find required Vulkan instance extension.");
		}
	}

	void Adapter::ensureDebugLayersExist()
	{
		uint32_t layerCount {0};
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> layers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

		for(auto requiredLayer : RequiredDebugLayers)
		{
			bool found = false;
			for(auto& layer : layers)
				if(std::strcmp(layer.layerName, requiredLayer) == 0)
				{
					found = true;
					break;
				}
			ctEnsure(found, "Failed to find required Vulkan validation layer.");
		}
	}

	void Adapter::initializeInstance()
	{
		auto appInfo {getAppInfo()};
		auto loggerInfo {getLoggerInfo()};

		VkInstanceCreateInfo instanceInfo {};
		instanceInfo.sType			  = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceInfo.pNext			  = &loggerInfo;
		instanceInfo.pApplicationInfo = &appInfo;
#if CT_DEBUG
		instanceInfo.enabledLayerCount	 = countOf(RequiredDebugLayers);
		instanceInfo.ppEnabledLayerNames = RequiredDebugLayers;
#endif
		instanceInfo.enabledExtensionCount	 = countOf(RequiredInstanceExtensions);
		instanceInfo.ppEnabledExtensionNames = RequiredInstanceExtensions;
		ctEnsureResult(vkCreateInstance(&instanceInfo, nullptr, &Instance), "Failed to create Vulkan instance.");

#if CT_DEBUG
		auto func {vkGetInstanceProcAddr(Instance, "vkCreateDebugUtilsMessengerEXT")};
		auto vkCreateDebugUtilsMessengerEXT {reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(func)};
		ctEnsureResult(vkCreateDebugUtilsMessengerEXT(Instance, &loggerInfo, nullptr, &Logger),
					   "Failed to create Vulkan logger.");
#endif
	}

}
