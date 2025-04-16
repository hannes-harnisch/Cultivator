#include "Surface.hpp"

#include "graphics/vulkan/Util.hpp"

namespace cltv {

Surface::Surface(const DeviceContext* ctx, const Window& window) :
	ctx_(ctx) {
	VkWin32SurfaceCreateInfoKHR surface_info {
		.sType	   = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		.pNext	   = nullptr,
		.flags	   = 0,
		.hinstance = window.get_instance_handle(),
		.hwnd	   = window.get_hwnd(),
	};
	VkResult result = ctx->lib.vkCreateWin32SurfaceKHR(ctx->instance(), &surface_info, nullptr, &surface_);
	require_vk_result(result, "failed to create Vulkan surface");
}

bool Surface::can_present_with_queue(uint32_t queue_family) {
	return ctx_->lib.vkGetPhysicalDeviceWin32PresentationSupportKHR(ctx_->physical_device(), queue_family);
}

} // namespace cltv
