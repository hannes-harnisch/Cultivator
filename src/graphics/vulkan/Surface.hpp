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
		return _surface;
	}

private:
	const DeviceContext* _ctx;
	VkSurfaceKHR _surface;
};

} // namespace cltv
