#include "PCH.hh"

#include "Utils/Assert.hh"
#include "Vendor/Vulkan/Vulkan.Surface.hh"
#include "Vulkan.GPUContext.hh"

namespace ct::vulkan
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

			std::printf("\n%s\n", pCallbackData->pMessage);
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
		Device.destroy();

#if CT_DEBUG
		Instance.destroyDebugUtilsMessengerEXT(Logger, {}, Loader::get());
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

	void GPUContext::initializeInstance(vk::DebugUtilsMessengerCreateInfoEXT const& loggerInfo)
	{
		ensureInstanceExtensionsExist();
		ensureLayersExist();

		auto appInfo	  = fillAppInfo();
		auto instanceInfo = vk::InstanceCreateInfo()
								.setPNext(&loggerInfo)
								.setPApplicationInfo(&appInfo)
								.setPEnabledLayerNames(RequiredLayers)
								.setPEnabledExtensionNames(RequiredInstanceExtensions);
		auto [res, instance] = vk::createInstance(instanceInfo);
		ctEnsureResult(res, "Failed to create Vulkan instance.");
		Instance = instance;
	}

	void GPUContext::initializeLoaderWithoutDevice()
	{
		Loader::loader = vk::DispatchLoaderDynamic(Instance, vkGetInstanceProcAddr);
	}

	void GPUContext::initializeLogger(vk::DebugUtilsMessengerCreateInfoEXT const& loggerInfo)
	{
#if CT_DEBUG
		auto [res, logger] = Instance.createDebugUtilsMessengerEXT(loggerInfo, nullptr, Loader::get());
		ctEnsureResult(res, "Failed to create Vulkan logger.");
		Logger = logger;
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
		auto [res, adapters] = Instance.enumeratePhysicalDevices(Loader::get());
		ctEnsureResult(res, "Failed to enumerate Vulkan adapters.");

		for(auto adapter : adapters)
		{
			auto properties = adapter.getProperties(Loader::get());

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

		auto families = queryQueueFamilies(Adapter);
		std::array queuePriorities {1.0f};
		vk::DeviceQueueCreateInfo graphicsQueueInfo({}, families.Graphics, queuePriorities);
		vk::DeviceQueueCreateInfo presentQueueInfo({}, families.Present, queuePriorities);

		std::vector queueInfos {graphicsQueueInfo};
		if(families.Graphics != families.Present)
			queueInfos.push_back(presentQueueInfo);

		auto features	= vk::PhysicalDeviceFeatures().setShaderImageGatherExtended(true);
		auto deviceInfo = vk::DeviceCreateInfo()
							  .setQueueCreateInfos(queueInfos)
							  .setPEnabledLayerNames(RequiredLayers)
							  .setPEnabledExtensionNames(RequiredDeviceExtensions)
							  .setPEnabledFeatures(&features);
		auto [res, device] = Adapter.createDevice(deviceInfo, nullptr, Loader::get());
		ctEnsureResult(res, "Failed to create Vulkan device.");
		Device = device;

		GraphicsQueue = Queue(families.Graphics);
		PresentQueue  = Queue(families.Present);
	}

	void GPUContext::ensureDeviceExtensionsExist()
	{
		auto [res, extensions] = Adapter.enumerateDeviceExtensionProperties(nullptr, Loader::get());
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
		Loader::loader = vk::DispatchLoaderDynamic(Instance, vkGetInstanceProcAddr, Device, vkGetDeviceProcAddr);
	}
}
