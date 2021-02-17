#pragma once

#include "PCH.hh"

namespace ct::vulkan
{
	class GraphicsPlatform
	{
	public:
		inline static GraphicsPlatform& get()
		{
			return *Singleton;
		}

		GraphicsPlatform();
		~GraphicsPlatform();

		inline vk::Instance instance()
		{
			return Instance;
		}

		inline vk::PhysicalDevice adapter()
		{
			return Adapter;
		}

		inline vk::Device device()
		{
			return Device;
		}

		GraphicsPlatform(const GraphicsPlatform&) = delete;
		GraphicsPlatform& operator=(const GraphicsPlatform&) = delete;

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

		static inline GraphicsPlatform* Singleton;

		vk::Instance Instance;
		vk::DebugUtilsMessengerEXT Logger;
		vk::PhysicalDevice Adapter;
		vk::Device Device;
		vk::Queue GraphicsQueue;
		vk::Queue PresentationQueue;

		struct QueueFamilyIndices
		{
			const uint32_t Graphics;
			const uint32_t Presentation;
		};

		void ensureInstanceExtensionsExist();
		void ensureLayersExist();
		void initializeInstance();
		void initializeAdapter();
		void ensureDeviceExtensionsExist();
		void initializeDevice();
		QueueFamilyIndices queryQueueFamilies();
		vk::DeviceCreateInfo fillDeviceInfo(const std::vector<vk::DeviceQueueCreateInfo>& queueInfos);
	};
}
