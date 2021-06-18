#pragma once

#include "PCH.hh"

#include "Utils/Singleton.hh"
#include "Vulkan.Queue.hh"

namespace ct
{
	class GPUContext final : public Singleton<GPUContext>
	{
	public:
		static vk::Instance instance()
		{
			return SingletonInstance->instanceHandle;
		}

		static vk::PhysicalDevice adapter()
		{
			return SingletonInstance->adapterHandle;
		}

		static vk::Device device()
		{
			return SingletonInstance->deviceHandle;
		}

		static vk::PhysicalDeviceProperties deviceProperties()
		{
			return SingletonInstance->deviceProps;
		}

		static Queue graphicsQueue()
		{
			return SingletonInstance->graphicsQueueHandle;
		}

		static Queue presentQueue()
		{
			return SingletonInstance->presentQueueHandle;
		}

		GPUContext();
		~GPUContext();

		GPUContext(GPUContext const&) = delete;
		GPUContext& operator=(GPUContext const&) = delete;

	private:
		static inline std::vector<char const*> const RequiredLayers
		{
#if CT_DEBUG
			"VK_LAYER_KHRONOS_validation"
#endif
		};

		static constexpr std::array RequiredInstanceExtensions
		{
			VK_KHR_SURFACE_EXTENSION_NAME,

#if CT_DEBUG
				VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif

#if CT_SYSTEM_WINDOWS
				VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
		};

		static constexpr std::array RequiredDeviceExtensions {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

		vk::DynamicLoader dl;
		vk::Instance instanceHandle;
#if CT_DEBUG
		vk::DebugUtilsMessengerEXT loggerHandle;
#endif
		vk::PhysicalDevice adapterHandle;
		vk::Device deviceHandle;
		vk::PhysicalDeviceProperties deviceProps;
		Queue graphicsQueueHandle;
		Queue presentQueueHandle;

		void initializeInstance(vk::DebugUtilsMessengerCreateInfoEXT const& loggerInfo);
		void ensureInstanceExtensionsExist();
		void ensureLayersExist();
		void initializeAdapter();
		void initializeDeviceAndQueues();
		void ensureFeaturesExist(vk::PhysicalDeviceFeatures const& required);
		void ensureDeviceExtensionsExist();
	};
}
