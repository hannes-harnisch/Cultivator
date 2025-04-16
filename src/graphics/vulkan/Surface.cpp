#include "Surface.hpp"

#include "graphics/vulkan/Util.hpp"

namespace cltv {

Surface::~Surface() {
	ctx_->lib.vkDestroySurfaceKHR(ctx_->instance(), surface_, nullptr);
}

bool Surface::supported_by_queue(uint32_t queue_family) {
	VkBool32 support;

	VkResult result = ctx_->lib.vkGetPhysicalDeviceSurfaceSupportKHR(ctx_->physical_device(), queue_family, surface_, &support);
	require_vk_result(result, "failed to query Vulkan surface support");

	return support;
}

} // namespace cltv
