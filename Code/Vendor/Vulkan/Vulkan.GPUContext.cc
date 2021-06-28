#include "PCH.hh"

#include "Utils/Assert.hh"
#include "Vulkan.GPUContext.hh"
#include "Vulkan.Surface.hh"
#include "Vulkan.Utils.hh"

namespace ct
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

	GPUContext::GPUContext()
	{
		auto vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
		VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

		vk::DebugUtilsMessengerCreateInfoEXT loggerInfo {
			.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
							   vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
							   vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
			.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
						   vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
			.pfnUserCallback = logDebug,
		};
		initializeInstance(loggerInfo);
		VULKAN_HPP_DEFAULT_DISPATCHER.init(instanceHandle);

#if CT_DEBUG
		auto [res, logger] = instanceHandle.createDebugUtilsMessengerEXT(loggerInfo);
		ctEnsureResult(res, "Failed to create Vulkan logger.");
		loggerHandle = logger;
#endif

		initializeAdapter();
		initializeDeviceAndQueues();
		VULKAN_HPP_DEFAULT_DISPATCHER.init(deviceHandle);
	}

	GPUContext::~GPUContext()
	{
		ctEnsureResult(deviceHandle.waitIdle(), "Failed to wait for idle device.");

		deviceHandle.destroy();
#if CT_DEBUG
		instanceHandle.destroyDebugUtilsMessengerEXT(loggerHandle);
#endif
		instanceHandle.destroy();
	}

	void GPUContext::initializeInstance(vk::DebugUtilsMessengerCreateInfoEXT const& loggerInfo)
	{
		ensureInstanceExtensionsExist();
		ensureLayersExist();

		auto enabled = vk::ValidationFeatureEnableEXT::eBestPractices;
		vk::ValidationFeaturesEXT features {
			.pNext						   = &loggerInfo,
			.enabledValidationFeatureCount = 1,
			.pEnabledValidationFeatures	   = &enabled,
		};

		vk::ApplicationInfo appInfo {
			.pApplicationName	= CT_APP_NAME,
			.applicationVersion = VK_MAKE_VERSION(0, 0, 1),
			.pEngineName		= CT_APP_NAME,
			.engineVersion		= VK_MAKE_VERSION(0, 0, 1),
			.apiVersion			= VK_API_VERSION_1_0,
		};
		void* instanceNext = nullptr;
#if CT_DEBUG
		instanceNext = &features;
#endif
		vk::InstanceCreateInfo info {
			.pNext					 = instanceNext,
			.pApplicationInfo		 = &appInfo,
			.enabledLayerCount		 = count(RequiredLayers),
			.ppEnabledLayerNames	 = RequiredLayers.data(),
			.enabledExtensionCount	 = count(RequiredInstanceExtensions),
			.ppEnabledExtensionNames = RequiredInstanceExtensions.data(),
		};
		auto [res, instance] = vk::createInstance(info);
		ctEnsureResult(res, "Failed to create Vulkan instance.");
		instanceHandle = instance;
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
		auto [res, adapters] = instanceHandle.enumeratePhysicalDevices();
		ctEnsureResult(res, "Failed to enumerate Vulkan adapters.");

		for(auto adapter : adapters)
		{
			deviceProps = adapter.getProperties();

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

			for(auto& queueFamily : adapter.getQueueFamilyProperties())
			{
				if(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
					graphicsFamily = index;

				auto [res, supports] = adapter.getSurfaceSupportKHR(index, dummy.handle());
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
		vk::DeviceQueueCreateInfo graphicsQueueInfo {
			.queueFamilyIndex = families.Graphics,
			.queueCount		  = 1,
			.pQueuePriorities = queuePriorities.data(),
		};
		vk::DeviceQueueCreateInfo presentQueueInfo {
			.queueFamilyIndex = families.Present,
			.queueCount		  = 1,
			.pQueuePriorities = queuePriorities.data(),
		};

		std::vector queueInfos {graphicsQueueInfo};
		if(families.Graphics != families.Present)
			queueInfos.push_back(presentQueueInfo);

		vk::PhysicalDeviceFeatures requiredFeatures {
			.samplerAnisotropy		   = true,
			.shaderImageGatherExtended = true,
		};
		ensureFeaturesExist(requiredFeatures);

		vk::DeviceCreateInfo info {
			.queueCreateInfoCount	 = count(queueInfos),
			.pQueueCreateInfos		 = queueInfos.data(),
			.enabledLayerCount		 = count(RequiredLayers),
			.ppEnabledLayerNames	 = RequiredLayers.data(),
			.enabledExtensionCount	 = count(RequiredDeviceExtensions),
			.ppEnabledExtensionNames = RequiredDeviceExtensions.data(),
			.pEnabledFeatures		 = &requiredFeatures,
		};
		auto [res, device] = adapterHandle.createDevice(info);
		ctEnsureResult(res, "Failed to create Vulkan device.");
		deviceHandle = device;

		graphicsQueueHandle = Queue(families.Graphics);
		presentQueueHandle	= Queue(families.Present);
	}

	void GPUContext::ensureFeaturesExist(vk::PhysicalDeviceFeatures const& required)
	{
		using FeatureArray = std::array<vk::Bool32, sizeof(vk::PhysicalDeviceFeatures) / sizeof(vk::Bool32)>;

		auto requiredFeatures  = std::bit_cast<FeatureArray>(required);
		auto availableFeatures = std::bit_cast<FeatureArray>(adapterHandle.getFeatures());

		auto available = std::begin(availableFeatures);
		for(auto featureRequired : requiredFeatures)
		{
			if(featureRequired)
				ctEnsure(*available, "Required feature not available.");
			++available;
		}
	}

	void GPUContext::ensureDeviceExtensionsExist()
	{
		auto [res, extensions] = adapterHandle.enumerateDeviceExtensionProperties();
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
}
