#include "Surface.hpp"

#include "graphics/vulkan/Util.hpp"

namespace cltv {

Surface::Surface(const DeviceContext* ctx, const Window& window) :
	_ctx(ctx) {
	VkWin32SurfaceCreateInfoKHR surface_info {
		.sType	   = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		.pNext	   = nullptr,
		.flags	   = 0,
		.hinstance = window.get_instance_handle(),
		.hwnd	   = window.get_hwnd(),
	};
	VkResult result = ctx->lib.vkCreateWin32SurfaceKHR(ctx->instance(), &surface_info, nullptr, &_surface);
	require_vk_result(result, "failed to create Vulkan surface");
}

} // namespace cltv
