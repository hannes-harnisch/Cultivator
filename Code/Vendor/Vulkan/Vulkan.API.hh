#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_ASSERT
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#if CT_SYSTEM_WINDOWS
	#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vulkan.hpp>

namespace vk
{
	inline DispatchLoaderDynamic defaultDispatchLoaderDynamic;
}
