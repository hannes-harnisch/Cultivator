#pragma once

#include "Platform/Vulkan/Vulkan.API.hh"

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

		VkInstance Instance;
		VkDebugUtilsMessengerEXT Logger;

		void ensureInstanceExtensionsExist();
		void ensureDebugLayersExist();
		void initializeInstance();
	};
}
