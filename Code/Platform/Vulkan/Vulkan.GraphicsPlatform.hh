#pragma once

#include "PCH.hh"

namespace ct::vulkan
{
	class GraphicsPlatform
	{
	public:
		GraphicsPlatform();
		~GraphicsPlatform();

		GraphicsPlatform(const GraphicsPlatform&) = delete;
		GraphicsPlatform& operator=(const GraphicsPlatform&) = delete;

		GraphicsPlatform(GraphicsPlatform&& other) = delete;
		GraphicsPlatform& operator=(GraphicsPlatform&& other) = delete;

		static inline GraphicsPlatform& get()
		{
			return *Singleton;
		}

		inline vk::Instance instance()
		{
			return Instance;
		}

	private:
		static constexpr const char* RequiredDebugLayers[] {"VK_LAYER_KHRONOS_validation"};
		static constexpr const char* RequiredInstanceExtensions[]
		{
			VK_KHR_SURFACE_EXTENSION_NAME,

#if CT_DEBUG
				VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif

#if CT_SYSTEM_WINDOWS
				VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
		};

		static inline GraphicsPlatform* Singleton;

		vk::Instance Instance;
		vk::DebugUtilsMessengerEXT Logger;
		vk::PhysicalDevice PhysicalDevice;
		vk::Device Device;

		void ensureInstanceExtensionsExist();
		void ensureDebugLayersExist();
		void initializeInstance();
		void initializePhysicalDevice();
	};
}
