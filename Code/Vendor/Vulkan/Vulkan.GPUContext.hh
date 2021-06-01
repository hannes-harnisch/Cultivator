﻿#pragma once

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

		GPUContext(const GPUContext&) = delete;
		GPUContext& operator=(const GPUContext&) = delete;

	private:
		static inline const std::vector<const char*> RequiredLayers
		{
#if CT_DEBUG
			"VK_LAYER_KHRONOS_validation"
#endif
		};

		static inline const std::array RequiredInstanceExtensions
		{
			VK_KHR_SURFACE_EXTENSION_NAME,

#if CT_DEBUG
				VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif

#if CT_SYSTEM_WINDOWS
				VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
		};

		static inline const std::array RequiredDeviceExtensions {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

		vk::Instance Instance;
		vk::DebugUtilsMessengerEXT Logger;
		vk::PhysicalDevice Adapter;
		vk::Device Device;
		Queue GraphicsQueue;
		Queue PresentQueue;

		void initializeInstance(const vk::DebugUtilsMessengerCreateInfoEXT& loggerInfo);
		void initializeLogger(const vk::DebugUtilsMessengerCreateInfoEXT& loggerInfo);
		void ensureInstanceExtensionsExist();
		void ensureLayersExist();
		void initializeAdapter();
		void initializeDeviceAndQueues();
		void ensureDeviceExtensionsExist();
	};

	class Loader final
	{
	public:
		static vk::DispatchLoaderDynamic& getDeviceless()
		{
			static vk::DispatchLoaderDynamic loader(GPUContext::instance(), vkGetInstanceProcAddr);
			return loader;
		}

		static vk::DispatchLoaderDynamic& get()
		{
			static vk::DispatchLoaderDynamic loader(GPUContext::instance(), vkGetInstanceProcAddr, GPUContext::device(),
													vkGetDeviceProcAddr);
			return loader;
		}

		Loader() = delete;
	};
}
