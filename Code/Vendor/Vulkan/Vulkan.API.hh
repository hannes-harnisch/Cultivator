#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS

#if CT_SYSTEM_WINDOWS
	#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vulkan.hpp>
