#pragma once

#include "app/Window.hpp"
#include "graphics/vulkan/DeviceContext.hpp"

namespace cltv {

class Surface {
public:
	Surface(const DeviceContext* ctx, const Window& window);
	~Surface();

	bool can_present_with_queue(uint32_t queue_family);
	bool supported_by_queue(uint32_t queue_family);

	VkSurfaceKHR get() const {
		return surface_;
	}

private:
	const DeviceContext* ctx_;
	VkSurfaceKHR surface_;
};

} // namespace cltv
