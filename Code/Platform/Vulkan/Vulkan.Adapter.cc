#include "PCH.hh"
#include "Platform/Vulkan/Vulkan.Adapter.hh"

#include "Assert.hh"

namespace ct::vulkan
{
	namespace
	{
		vk::ApplicationInfo getAppInfo()
		{
			return vk::ApplicationInfo()
				.setPApplicationName(CT_APP_NAME)
				.setApplicationVersion(VK_MAKE_VERSION(0, 0, 1))
				.setPEngineName(CT_APP_NAME)
				.setEngineVersion(VK_MAKE_VERSION(0, 0, 1))
				.setApiVersion(VK_API_VERSION_1_0);
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

		vk::DebugUtilsMessengerCreateInfoEXT getLoggerInfo()
		{
			return vk::DebugUtilsMessengerCreateInfoEXT()
				.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
									vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
									vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
				.setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
								vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
								vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
				.setPfnUserCallback(logDebug);
		}
	}

	Adapter::Adapter()
	{
		ctEnsure(!Singleton, "Adapter can only be instantiated once.");
		Singleton = this;

		ensureInstanceExtensionsExist();
#if CT_DEBUG
		ensureDebugLayersExist();
#endif
		initializeInstance();
		initializePhysicalDevice();
	}

	Adapter::Adapter(Adapter&& other) noexcept
	{
		*this = std::move(other);
	}

	Adapter::~Adapter()
	{
#if CT_DEBUG
		vk::DispatchLoaderDynamic dispatch(Instance, vkGetInstanceProcAddr);
		Instance.destroyDebugUtilsMessengerEXT(Logger, nullptr, dispatch);
#endif
	}

	Adapter& Adapter::operator=(Adapter&& other) noexcept
	{
		std::swap(Instance, other.Instance);
		std::swap(Logger, other.Logger);
		std::swap(PhysicalDevice, other.PhysicalDevice);
		return *this;
	}

	void Adapter::ensureInstanceExtensionsExist()
	{
		auto extensions {vk::enumerateInstanceExtensionProperties()};

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
		auto layers {vk::enumerateInstanceLayerProperties()};

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
		auto info
		{
			vk::InstanceCreateInfo()
				.setPNext(&loggerInfo)
				.setPApplicationInfo(&appInfo)
				.setEnabledExtensionCount(uint32_t(std::size(RequiredInstanceExtensions)))
				.setPpEnabledExtensionNames(RequiredInstanceExtensions)
#if CT_DEBUG
				.setEnabledLayerCount(uint32_t(std::size(RequiredDebugLayers)))
				.setPpEnabledLayerNames(RequiredDebugLayers)
#endif
		};
		Instance = vk::createInstance(info);
		ctEnsure(Instance, "Failed to create Vulkan instance.");

#if CT_DEBUG
		vk::DispatchLoaderDynamic dispatch(Instance, vkGetInstanceProcAddr);
		Logger = Instance.createDebugUtilsMessengerEXT(loggerInfo, nullptr, dispatch);
		ctEnsure(Logger, "Failed to create Vulkan logger.");
#endif
	}

	void Adapter::initializePhysicalDevice()
	{
		auto devices {Instance.enumeratePhysicalDevices()};

		for(auto device : devices)
		{
			auto properties {device.getProperties()};

			if(properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu)
				PhysicalDevice = device;
			if(properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
			{
				PhysicalDevice = device;
				break;
			}
		}
	}
}
