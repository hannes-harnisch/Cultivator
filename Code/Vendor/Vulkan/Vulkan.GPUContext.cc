#include "PCH.hh"

#include "Utils/Assert.hh"
#include "Vulkan.GPUContext.hh"
#include "Vulkan.Surface.hh"
#include "Vulkan.Utils.hh"

namespace ct
{
	namespace
	{
		VKAPI_ATTR VkBool32 VKAPI_CALL logDebug(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
												VkDebugUtilsMessageTypeFlagsEXT,
												VkDebugUtilsMessengerCallbackDataEXT const* pCallbackData,
												void*)
		{
			if(messageSeverity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
				return false;

			std::string_view msg = pCallbackData->pMessage;
			if(msg.starts_with("loaderAddLayerProperties"))
				return false;

			std::printf("\n%s\n", pCallbackData->pMessage);
			return false;
		}

		vk::DebugUtilsMessengerCreateInfoEXT fillLoggerInfo()
		{
			vk::DebugUtilsMessengerCreateInfoEXT info;
			info.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
								   vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
								   vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
			info.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
							   vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
							   vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
			info.pfnUserCallback = logDebug;
			return info;
		}
	}

	GPUContext::GPUContext()
	{
		auto loggerInfo = fillLoggerInfo();
		initializeInstance(loggerInfo);
		initializeLoaderWithoutDevice();
		initializeLogger(loggerInfo);
		initializeAdapter();
		initializeDeviceAndQueues();
		recreateLoaderWithDevice();
	}

	GPUContext::~GPUContext()
	{
		ctEnsureResult(deviceHandle.waitIdle(Loader::get()), "Failed to wait for idle device.");

		deviceHandle.destroy({}, Loader::get());
#if CT_DEBUG
		instanceHandle.destroyDebugUtilsMessengerEXT(loggerHandle, {}, Loader::get());
#endif
		instanceHandle.destroy({}, Loader::get());
	}

	namespace
	{
		vk::ApplicationInfo fillAppInfo()
		{
			vk::ApplicationInfo info;
			info.pApplicationName	= CT_APP_NAME;
			info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
			info.pEngineName		= CT_APP_NAME;
			info.engineVersion		= VK_MAKE_VERSION(0, 0, 1);
			info.apiVersion			= VK_API_VERSION_1_0;
			return info;
		}
	}

	void GPUContext::initializeInstance(vk::DebugUtilsMessengerCreateInfoEXT const& loggerInfo)
	{
		ensureInstanceExtensionsExist();
		ensureLayersExist();

		auto enabled = vk::ValidationFeatureEnableEXT::eBestPractices;
		vk::ValidationFeaturesEXT features;
		features.pNext						   = &loggerInfo;
		features.enabledValidationFeatureCount = 1;
		features.pEnabledValidationFeatures	   = &enabled;

		auto appInfo = fillAppInfo();

		vk::InstanceCreateInfo info;
#if CT_DEBUG
		info.pNext = &loggerInfo; // &validationFeatures
#endif
		info.pApplicationInfo		 = &appInfo;
		info.enabledLayerCount		 = count(RequiredLayers);
		info.ppEnabledLayerNames	 = RequiredLayers.data();
		info.enabledExtensionCount	 = count(RequiredInstanceExtensions);
		info.ppEnabledExtensionNames = RequiredInstanceExtensions.data();

		auto [res, instance] = vk::createInstance(info);
		ctEnsureResult(res, "Failed to create Vulkan instance.");
		instanceHandle = instance;
	}

	void GPUContext::initializeLoaderWithoutDevice()
	{
		Loader::loader = vk::DispatchLoaderDynamic(instanceHandle, vkGetInstanceProcAddr);
	}

	void GPUContext::initializeLogger(vk::DebugUtilsMessengerCreateInfoEXT const& loggerInfo)
	{
#if CT_DEBUG
		auto [res, logger] = instanceHandle.createDebugUtilsMessengerEXT(loggerInfo, nullptr, Loader::get());
		ctEnsureResult(res, "Failed to create Vulkan logger.");
		loggerHandle = logger;
#endif
	}

	void GPUContext::ensureInstanceExtensionsExist()
	{
		auto [res, extensions] = vk::enumerateInstanceExtensionProperties();
		ctEnsureResult(res, "Failed to enumerate Vulkan instance extensions.");

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

	void GPUContext::ensureLayersExist()
	{
		auto [res, layers] = vk::enumerateInstanceLayerProperties();
		ctEnsureResult(res, "Failed to enumerate Vulkan layers.");

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

	void GPUContext::initializeAdapter()
	{
		auto [res, adapters] = instanceHandle.enumeratePhysicalDevices(Loader::get());
		ctEnsureResult(res, "Failed to enumerate Vulkan adapters.");

		for(auto adapter : adapters)
		{
			deviceProps = adapter.getProperties(Loader::get());

			if(deviceProps.deviceType == vk::PhysicalDeviceType::eIntegratedGpu)
				adapterHandle = adapter;
			if(deviceProps.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
			{
				adapterHandle = adapter;
				break;
			}
		}
	}

	namespace
	{
		struct QueueFamilies
		{
			uint32_t Graphics;
			uint32_t Present;
		};

		QueueFamilies queryQueueFamilies(vk::PhysicalDevice adapter)
		{
			std::optional<uint32_t> graphicsFamily, presentFamily;

			uint32_t index {};
			auto dummy = Surface::makeDummy();

			for(auto& queueFamily : adapter.getQueueFamilyProperties(Loader::get()))
			{
				if(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
					graphicsFamily = index;

				auto [res, supports] = adapter.getSurfaceSupportKHR(index, dummy.handle(), Loader::get());
				ctEnsureResult(res, "Failed to query for Vulkan surface support.");
				if(supports)
					presentFamily = index;

				++index;
			}
			ctEnsure(graphicsFamily, "The GPU driver does not support graphics queues.");
			ctEnsure(presentFamily, "The GPU driver does not support present queues.");
			return {*graphicsFamily, *presentFamily};
		}
	}

	void GPUContext::initializeDeviceAndQueues()
	{
		ensureDeviceExtensionsExist();

		auto families = queryQueueFamilies(adapterHandle);
		std::array queuePriorities {1.0f};
		vk::DeviceQueueCreateInfo graphicsQueueInfo({}, families.Graphics, queuePriorities);
		vk::DeviceQueueCreateInfo presentQueueInfo({}, families.Present, queuePriorities);

		std::vector queueInfos {graphicsQueueInfo};
		if(families.Graphics != families.Present)
			queueInfos.push_back(presentQueueInfo);

		vk::PhysicalDeviceFeatures features;
		features.shaderImageGatherExtended = true;
		features.samplerAnisotropy		   = true;

		vk::DeviceCreateInfo info;
		info.queueCreateInfoCount	 = count(queueInfos);
		info.pQueueCreateInfos		 = queueInfos.data();
		info.enabledLayerCount		 = count(RequiredLayers);
		info.ppEnabledLayerNames	 = RequiredLayers.data();
		info.enabledExtensionCount	 = count(RequiredDeviceExtensions);
		info.ppEnabledExtensionNames = RequiredDeviceExtensions.data();
		info.pEnabledFeatures		 = &features;

		auto [res, device] = adapterHandle.createDevice(info, nullptr, Loader::get());
		ctEnsureResult(res, "Failed to create Vulkan device.");
		deviceHandle = device;

		graphicsQueueHandle = Queue(families.Graphics);
		presentQueueHandle	= Queue(families.Present);
	}

	void GPUContext::ensureDeviceExtensionsExist()
	{
		auto [res, extensions] = adapterHandle.enumerateDeviceExtensionProperties(nullptr, Loader::get());
		ctEnsureResult(res, "Failed to enumerate Vulkan device extensions.");

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

	void GPUContext::recreateLoaderWithDevice()
	{
		Loader::loader = vk::DispatchLoaderDynamic(instanceHandle, vkGetInstanceProcAddr, deviceHandle, vkGetDeviceProcAddr);
	}
}
