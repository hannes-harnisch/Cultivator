#include "PCH.hh"
#include "Vulkan.GraphicsContext.hh"

#include "Utils/Assert.hh"
#include "Vendor/Vulkan/Vulkan.Surface.hh"

namespace ct::vulkan
{
	namespace
	{
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

		vk::DebugUtilsMessengerCreateInfoEXT fillLoggerInfo()
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

	GraphicsContext::GraphicsContext()
	{
		ctEnsure(!Singleton, "GraphicsContext can only be instantiated once.");
		Singleton = this;

		auto loggerInfo {fillLoggerInfo()};
		initializeInstance(loggerInfo);
		initializeLogger(loggerInfo);
		initializeAdapter();
		initializeDeviceAndQueues();
	}

	GraphicsContext::~GraphicsContext()
	{
		Device.destroy();

#if CT_DEBUG
		vk::DispatchLoaderDynamic dispatch(Instance, vkGetInstanceProcAddr);
		Instance.destroyDebugUtilsMessengerEXT(Logger, nullptr, dispatch);
#endif

		Instance.destroy();
	}

	namespace
	{
		vk::ApplicationInfo fillAppInfo()
		{
			return vk::ApplicationInfo()
				.setPApplicationName(CT_APP_NAME)
				.setApplicationVersion(VK_MAKE_VERSION(0, 0, 1))
				.setPEngineName(CT_APP_NAME)
				.setEngineVersion(VK_MAKE_VERSION(0, 0, 1))
				.setApiVersion(VK_API_VERSION_1_0);
		}
	}

	void GraphicsContext::initializeInstance(const vk::DebugUtilsMessengerCreateInfoEXT& loggerInfo)
	{
		ensureInstanceExtensionsExist();
		ensureLayersExist();

		auto appInfo {fillAppInfo()};
		auto instanceInfo {vk::InstanceCreateInfo()
							   .setPNext(&loggerInfo)
							   .setPApplicationInfo(&appInfo)
							   .setPEnabledLayerNames(RequiredLayers)
							   .setPEnabledExtensionNames(RequiredInstanceExtensions)};
		auto [result, instance] {vk::createInstance(instanceInfo)};
		ctEnsureResult(result, "Failed to create Vulkan instance.");
		Instance = instance;
	}

	void GraphicsContext::initializeLogger(const vk::DebugUtilsMessengerCreateInfoEXT& loggerInfo)
	{
#if CT_DEBUG
		vk::DispatchLoaderDynamic dispatch(Instance, vkGetInstanceProcAddr);
		auto [result, logger] {Instance.createDebugUtilsMessengerEXT(loggerInfo, nullptr, dispatch)};
		ctEnsureResult(result, "Failed to create Vulkan logger.");
		Logger = logger;
#endif
	}

	void GraphicsContext::ensureInstanceExtensionsExist()
	{
		auto [result, extensions] {vk::enumerateInstanceExtensionProperties()};
		ctEnsureResult(result, "Failed to enumerate Vulkan instance extensions.");

		for(auto requiredExtension : RequiredInstanceExtensions)
		{
			bool found {};
			for(auto& extension : extensions)
				if(std::strcmp(extension.extensionName, requiredExtension) == 0)
				{
					found = true;
					break;
				}
			ctEnsure(found, "Failed to find required Vulkan instance extension.");
		}
	}

	void GraphicsContext::ensureLayersExist()
	{
		auto [result, layers] {vk::enumerateInstanceLayerProperties()};
		ctEnsureResult(result, "Failed to enumerate Vulkan layers.");

		for(auto requiredLayer : RequiredLayers)
		{
			bool found {};
			for(auto& layer : layers)
				if(std::strcmp(layer.layerName, requiredLayer) == 0)
				{
					found = true;
					break;
				}
			ctEnsure(found, "Failed to find required Vulkan layer.");
		}
	}

	void GraphicsContext::initializeAdapter()
	{
		auto [result, adapters] {Instance.enumeratePhysicalDevices()};
		ctEnsureResult(result, "Failed to enumerate Vulkan adapters.");

		for(auto adapter : adapters)
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

	namespace
	{
		struct QueueFamilyIndices
		{
			uint32_t Graphics;
			uint32_t Present;
		};

		QueueFamilyIndices queryQueueFamilies(vk::PhysicalDevice adapter)
		{
			std::optional<uint32_t> graphicsFamily;
			std::optional<uint32_t> presentationFamily;

			uint32_t index {};
			auto dummy {Surface::makeDummy()};
			for(auto& queueFamily : adapter.getQueueFamilyProperties())
			{
				if(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
					graphicsFamily = index;

				auto [result, supportsSurfaces] {adapter.getSurfaceSupportKHR(index, dummy.handle())};
				ctEnsureResult(result, "Failed to query for Vulkan surface support.");
				if(supportsSurfaces)
					presentationFamily = index;

				++index;
			}
			ctEnsure(graphicsFamily, "The GPU driver does not support graphics queues.");
			ctEnsure(presentationFamily, "The GPU driver does not support presentation queues.");
			return {*graphicsFamily, *presentationFamily};
		}
	}

	void GraphicsContext::initializeDeviceAndQueues()
	{
		ensureDeviceExtensionsExist();

		auto families {queryQueueFamilies(Adapter)};
		const std::array<float, 1> queuePriorities {1.0f};
		vk::DeviceQueueCreateInfo graphicsQueueInfo({}, families.Graphics, queuePriorities);
		auto presentQueueInfo {graphicsQueueInfo};
		presentQueueInfo.setQueueFamilyIndex(families.Present);

		std::vector queueInfos {graphicsQueueInfo};
		if(families.Graphics != families.Present)
			queueInfos.push_back(presentQueueInfo);

		auto deviceInfo {vk::DeviceCreateInfo()
							 .setQueueCreateInfos(queueInfos)
							 .setPEnabledLayerNames(RequiredLayers)
							 .setPEnabledExtensionNames(RequiredDeviceExtensions)};
		auto [result, device] {Adapter.createDevice(deviceInfo)};
		ctEnsureResult(result, "Failed to create Vulkan device.");
		Device = device;

		GraphicsQueue = Queue(families.Graphics);
		PresentQueue  = Queue(families.Present);
	}

	void GraphicsContext::ensureDeviceExtensionsExist()
	{
		auto [result, extensions] {Adapter.enumerateDeviceExtensionProperties()};
		ctEnsureResult(result, "Failed to enumerate Vulkan device extensions.");

		for(auto requiredExtension : RequiredDeviceExtensions)
		{
			bool found {};
			for(auto& extension : extensions)
				if(std::strcmp(extension.extensionName, requiredExtension) == 0)
				{
					found = true;
					break;
				}
			ctEnsure(found, "Failed to find required Vulkan device extension.");
		}
	}
}
