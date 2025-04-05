#include "Surface.hpp"

#include "graphics/vulkan/Util.hpp"

namespace cltv {

Surface::Surface(const DeviceContext& ctx, Window& window) {
	VkWin32SurfaceCreateInfoKHR surface_info {
		.sType	   = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		.flags	   = 0,
		.hinstance = window.get_instance_handle(),
		.hwnd	   = window.get_hwnd(),
	};
	VkResult result = ctx._lib.vkCreateWin32SurfaceKHR(ctx._instance, &surface_info, nullptr, &_surface);
	require_vk_result(result, "failed to create Vulkan surface");
}

} // namespace cltv
