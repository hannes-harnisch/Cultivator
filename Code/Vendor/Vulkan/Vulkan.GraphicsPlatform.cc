#include "PCH.hh"
#include "Vulkan.GraphicsPlatform.hh"

#include "Assert.hh"
#include "Vendor/Vulkan/Vulkan.Surface.hh"

namespace ct::vulkan
{
	GraphicsPlatform::GraphicsPlatform()
	{
		ctEnsure(!Singleton, "GraphicsPlatform can only be instantiated once.");
		Singleton = this;

		initializeInstance();
		initializeAdapter();
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

	void GraphicsPlatform::initializeInstance()
	{
		ensureInstanceExtensionsExist();
		ensureLayersExist();
		auto appInfo {fillAppInfo()};
		auto loggerInfo {fillLoggerInfo()};
		auto instanceInfo {vk::InstanceCreateInfo()
							   .setPNext(&loggerInfo)
							   .setPApplicationInfo(&appInfo)
							   .setEnabledLayerCount(uint32_t(RequiredLayers.size()))
							   .setPpEnabledLayerNames(RequiredLayers.data())
							   .setEnabledExtensionCount(uint32_t(RequiredInstanceExtensions.size()))
							   .setPpEnabledExtensionNames(RequiredInstanceExtensions.data())};
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

	void GraphicsPlatform::ensureInstanceExtensionsExist()
	{
		auto extensions {vk::enumerateInstanceExtensionProperties()};
		ctEnsureResult(extensions.result, "Failed to enumerate Vulkan instance extensions.");

		for(auto requiredExtension : RequiredInstanceExtensions)
		{
			bool found {};
			for(auto& extension : extensions.value)
				if(std::strcmp(extension.extensionName, requiredExtension) == 0)
				{
					found = true;
					break;
				}
			ctEnsure(found, "Failed to find required Vulkan instance extension.");
		}
	}

	void GraphicsPlatform::ensureLayersExist()
	{
		auto layers {vk::enumerateInstanceLayerProperties()};
		ctEnsureResult(layers.result, "Failed to enumerate Vulkan layers.");

		for(auto requiredLayer : RequiredLayers)
		{
			bool found {};
			for(auto& layer : layers.value)
				if(std::strcmp(layer.layerName, requiredLayer) == 0)
				{
					found = true;
					break;
				}
			ctEnsure(found, "Failed to find required Vulkan layer.");
		}
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

	namespace
	{
		struct QueueFamilyIndices
		{
			uint32_t Graphics;
			uint32_t Present;
		};

		QueueFamilyIndices queryQueueFamilies(vk::PhysicalDevice adapter)
		{
			std::optional<uint32_t> graphicsFamilyIndex;
			std::optional<uint32_t> presentationFamilyIndex;

			uint32_t index {};
			auto dummy {Surface::makeDummy()};
			for(auto& queueFamily : adapter.getQueueFamilyProperties())
			{
				if(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
					graphicsFamilyIndex = index;

				auto surfaceSupport {adapter.getSurfaceSupportKHR(index, dummy.handle())};
				ctEnsureResult(surfaceSupport.result, "Failed to query for Vulkan surface support.");
				if(surfaceSupport.value)
					presentationFamilyIndex = index;

				++index;
			}
			ctEnsure(graphicsFamilyIndex, "The GPU driver does not support graphics queues.");
			ctEnsure(presentationFamilyIndex, "The GPU driver does not support presentation queues.");
			return {*graphicsFamilyIndex, *presentationFamilyIndex};
		}
	}

	void GraphicsPlatform::initializeDevice()
	{
		ensureDeviceExtensionsExist();
		auto indices {queryQueueFamilies(Adapter)};

		float queuePriorities[] {1.0f};
		auto graphicsQueueInfo {vk::DeviceQueueCreateInfo()
									.setQueueFamilyIndex(indices.Graphics)
									.setQueueCount(uint32_t(std::size(queuePriorities)))
									.setPQueuePriorities(queuePriorities)};
		auto presentQueueInfo(graphicsQueueInfo);
		presentQueueInfo.setQueueFamilyIndex(indices.Present);

		std::vector queueInfos {graphicsQueueInfo};
		if(indices.Graphics != indices.Present)
			queueInfos.push_back(presentQueueInfo);

		auto deviceInfo {fillDeviceInfo(queueInfos)};
		auto device {Adapter.createDevice(deviceInfo)};
		ctEnsureResult(device.result, "Failed to create Vulkan device.");
		Device = device.value;

		GraphicsQueue = Queue(indices.Graphics);
		PresentQueue  = Queue(indices.Present);
	}

	void GraphicsPlatform::ensureDeviceExtensionsExist()
	{
		auto extensions {Adapter.enumerateDeviceExtensionProperties()};
		ctEnsureResult(extensions.result, "Failed to enumerate Vulkan device extensions.");

		for(auto requiredExtension : RequiredDeviceExtensions)
		{
			bool found {};
			for(auto& extension : extensions.value)
				if(std::strcmp(extension.extensionName, requiredExtension) == 0)
				{
					found = true;
					break;
				}
			ctEnsure(found, "Failed to find required Vulkan device extension.");
		}
	}

	vk::DeviceCreateInfo GraphicsPlatform::fillDeviceInfo(const std::vector<vk::DeviceQueueCreateInfo>& queueInfos)
	{
		return vk::DeviceCreateInfo()
			.setQueueCreateInfoCount(uint32_t(queueInfos.size()))
			.setPQueueCreateInfos(queueInfos.data())
			.setEnabledLayerCount(uint32_t(RequiredLayers.size()))
			.setPpEnabledLayerNames(RequiredLayers.data())
			.setEnabledExtensionCount(uint32_t(RequiredDeviceExtensions.size()))
			.setPpEnabledExtensionNames(RequiredDeviceExtensions.data());
	}
}
