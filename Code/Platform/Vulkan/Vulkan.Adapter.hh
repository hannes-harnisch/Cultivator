#pragma once

#include "PCH.hh"

namespace ct::vulkan
{
	class Adapter
	{
	public:
		Adapter();
		Adapter(Adapter&& other) noexcept;
		~Adapter();
		Adapter& operator=(Adapter&& other) noexcept;

		Adapter(const Adapter&) = delete;
		Adapter& operator=(const Adapter&) = delete;

		static inline Adapter& get()
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

		static inline Adapter* Singleton;

		vk::Instance Instance;
		vk::DebugUtilsMessengerEXT Logger;
		vk::PhysicalDevice PhysicalDevice;

		void ensureInstanceExtensionsExist();
		void ensureDebugLayersExist();
		void initializeInstance();
		void initializePhysicalDevice();
	};
}
