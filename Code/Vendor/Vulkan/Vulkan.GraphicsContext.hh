#pragma once

#include "PCH.hh"
#include "Vendor/Vulkan/Vulkan.Queue.hh"

namespace ct::vulkan
{
	class GraphicsContext final
	{
	public:
		inline static GraphicsContext& get()
		{
			return *Singleton;
		}

		GraphicsContext();
		~GraphicsContext();

		inline vk::Instance instance() const
		{
			return Instance;
		}

		inline vk::PhysicalDevice adapter() const
		{
			return Adapter;
		}

		inline vk::Device device() const
		{
			return Device;
		}

		inline Queue graphicsQueue() const
		{
			return GraphicsQueue;
		}

		inline Queue presentQueue() const
		{
			return PresentQueue;
		}

		GraphicsContext(const GraphicsContext&) = delete;
		GraphicsContext& operator=(const GraphicsContext&) = delete;

	private:
		static inline const std::vector<const char*> RequiredLayers
		{
#if CT_DEBUG
			"VK_LAYER_KHRONOS_validation"
#endif
		};

		static inline const std::vector RequiredInstanceExtensions
		{
			VK_KHR_SURFACE_EXTENSION_NAME,

#if CT_DEBUG
				VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif

#if CT_SYSTEM_WINDOWS
				VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
		};

		static inline const std::vector RequiredDeviceExtensions {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

		static inline GraphicsContext* Singleton;

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
}
