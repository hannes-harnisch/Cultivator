#include "Surface.hpp"

#include "graphics/vulkan/Util.hpp"

namespace cltv {

Surface::~Surface() {
	_ctx->lib.vkDestroySurfaceKHR(_ctx->instance(), _surface, nullptr);
}

bool Surface::supported_by_queue(uint32_t queue_family) {
	VkBool32 support;

	VkResult result = _ctx->lib.vkGetPhysicalDeviceSurfaceSupportKHR(_ctx->physical_device(), queue_family, _surface, &support);
	require_vk_result(result, "failed to query Vulkan surface support");

	return support;
}

} // namespace cltv
