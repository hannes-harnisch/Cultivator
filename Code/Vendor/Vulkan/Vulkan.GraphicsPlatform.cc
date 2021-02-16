#include "PCH.hh"
#include "Vulkan.GraphicsPlatform.hh"

#include "Assert.hh"
#include "Vendor/Vulkan/Vulkan.Surface.hh"

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
			if(messageSeverity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
				return false;

			std::string messageType;
			switch(messageTypes)
			{
				case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: messageType = "GENERAL"; break;
				case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: messageType = "VALIDATION"; break;
				case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: messageType = "PERFORMANCE"; break;
			}
			std::printf("\n[DEBUG] [%s] %s\n", messageType.data(), pCallbackData->pMessage);
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

	GraphicsPlatform::GraphicsPlatform()
	{
		ctEnsure(!Singleton, "GraphicsPlatform can only be instantiated once.");
		Singleton = this;

		ensureInstanceExtensionsExist();
#if CT_DEBUG
		ensureDebugLayersExist();
#endif
		initializeInstance();
		initializeAdapter();
		ensureDeviceExtensionsExist();
		initializeDevice();
	}

	GraphicsPlatform::~GraphicsPlatform()
	{
		Device.destroy();

#if CT_DEBUG
		vk::DispatchLoaderDynamic dispatch(Instance, vkGetInstanceProcAddr);
		Instance.destroyDebugUtilsMessengerEXT(Logger, nullptr, dispatch);
#endif

		Instance.destroy();
	}

	void GraphicsPlatform::ensureInstanceExtensionsExist()
	{
		auto extensions {vk::enumerateInstanceExtensionProperties()};
		ctEnsureResult(extensions.result, "Failed to enumerate Vulkan instance extensions.");

		for(auto requiredExtension : RequiredInstanceExtensions)
		{
			bool found = false;
			for(auto& extension : extensions.value)
				if(std::strcmp(extension.extensionName, requiredExtension) == 0)
				{
					found = true;
					break;
				}
			ctEnsure(found, "Failed to find required Vulkan instance extension.");
		}
	}

	void GraphicsPlatform::ensureDebugLayersExist()
	{
		auto layers {vk::enumerateInstanceLayerProperties()};
		ctEnsureResult(layers.result, "Failed to enumerate Vulkan instance layers.");

		for(auto requiredLayer : RequiredDebugLayers)
		{
			bool found = false;
			for(auto& layer : layers.value)
				if(std::strcmp(layer.layerName, requiredLayer) == 0)
				{
					found = true;
					break;
				}
			ctEnsure(found, "Failed to find required Vulkan validation layer.");
		}
	}

	void GraphicsPlatform::initializeInstance()
	{
		auto appInfo {getAppInfo()};
		auto loggerInfo {getLoggerInfo()};
		auto instanceInfo = vk::InstanceCreateInfo()
								.setPNext(&loggerInfo)
								.setPApplicationInfo(&appInfo)
#if CT_DEBUG
								.setEnabledLayerCount(uint32_t(std::size(RequiredDebugLayers)))
								.setPpEnabledLayerNames(RequiredDebugLayers)
#endif
								.setEnabledExtensionCount(uint32_t(std::size(RequiredInstanceExtensions)))
								.setPpEnabledExtensionNames(RequiredInstanceExtensions);
		auto instance {vk::createInstance(instanceInfo)};
		ctEnsureResult(instance.result, "Failed to create Vulkan instance.");
		Instance = instance.value;

#if CT_DEBUG
		vk::DispatchLoaderDynamic dispatch(Instance, vkGetInstanceProcAddr);
		auto logger {Instance.createDebugUtilsMessengerEXT(loggerInfo, nullptr, dispatch)};
		ctEnsureResult(logger.result, "Failed to create Vulkan logger.");
		Logger = logger.value;
#endif
	}

	void GraphicsPlatform::initializeAdapter()
	{
		auto adapters {Instance.enumeratePhysicalDevices()};
		ctEnsureResult(adapters.result, "Failed to enumerate Vulkan adapters.");

		for(auto adapter : adapters.value)
		{
			auto properties {adapter.getProperties()};

			if(properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu)
				Adapter = adapter;
			if(properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
			{
				Adapter = adapter;
				break;
			}
		}
	}

	void GraphicsPlatform::ensureDeviceExtensionsExist()
	{
		auto extensions {Adapter.enumerateDeviceExtensionProperties()};
		ctEnsureResult(extensions.result, "Failed to enumerate Vulkan device extensions.");

		for(auto requiredExtension : RequiredDeviceExtensions)
		{
			bool found = false;
			for(auto& extension : extensions.value)
				if(std::strcmp(extension.extensionName, requiredExtension) == 0)
				{
					found = true;
					break;
				}
			ctEnsure(found, "Failed to find required Vulkan device extension.");
		}
	}

	void GraphicsPlatform::initializeDevice()
	{
		uint32_t graphicsFamilyIndex {UINT32_MAX};
		uint32_t presentationFamilyIndex {UINT32_MAX};

		uint32_t index {};
		auto dummySurface {Surface::makeDummy()};
		for(auto& queueFamily : Adapter.getQueueFamilyProperties())
		{
			if(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
				graphicsFamilyIndex = index;

			auto surfaceSupport {Adapter.getSurfaceSupportKHR(index, dummySurface.handle())};
			ctEnsureResult(surfaceSupport.result, "Failed to query for Vulkan surface support.");
			if(surfaceSupport.value)
				presentationFamilyIndex = index;

			index++;
		}
	}
}
