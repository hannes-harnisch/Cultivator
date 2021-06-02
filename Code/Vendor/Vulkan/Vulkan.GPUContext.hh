#pragma once

#include "PCH.hh"

#include "Utils/Singleton.hh"
#include "Vendor/Vulkan/Vulkan.Queue.hh"

namespace ct::vulkan
{
	class GPUContext final : public Singleton<GPUContext>
	{
	public:
		static vk::Instance instance()
		{
			return SingletonInstance->Instance;
		}

		static vk::PhysicalDevice adapter()
		{
			return SingletonInstance->Adapter;
		}

		static vk::Device device()
		{
			return SingletonInstance->Device;
		}

		static Queue graphicsQueue()
		{
			return SingletonInstance->GraphicsQueue;
		}

		static Queue presentQueue()
		{
			return SingletonInstance->PresentQueue;
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

		static inline std::array const RequiredInstanceExtensions
		{
			VK_KHR_SURFACE_EXTENSION_NAME,

#if CT_DEBUG
				VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif

#if CT_SYSTEM_WINDOWS
				VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
		};

		static inline std::array const RequiredDeviceExtensions {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

		vk::Instance Instance;
		vk::DebugUtilsMessengerEXT Logger;
		vk::PhysicalDevice Adapter;
		vk::Device Device;
		Queue GraphicsQueue;
		Queue PresentQueue;

		void initializeInstance(vk::DebugUtilsMessengerCreateInfoEXT const& loggerInfo);
		void initializeLoaderWithoutDevice();
		void initializeLogger(vk::DebugUtilsMessengerCreateInfoEXT const& loggerInfo);
		void ensureInstanceExtensionsExist();
		void ensureLayersExist();
		void initializeAdapter();
		void initializeDeviceAndQueues();
		void ensureDeviceExtensionsExist();
		void recreateLoaderWithDevice();
	};

	class Loader final
	{
		friend GPUContext;

	public:
		static vk::DispatchLoaderDynamic& get()
		{
			return loader;
		}

		Loader() = delete;

	private:
		static inline vk::DispatchLoaderDynamic loader;
	};
}
