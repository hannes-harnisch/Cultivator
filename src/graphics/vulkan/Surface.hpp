#pragma once

#include "app/Window.hpp"
#include "graphics/vulkan/DeviceContext.hpp"

namespace cltv {

class Surface {
public:
	Surface(const DeviceContext& ctx, Window& window);

	~Surface() {
		assert(_surface == VK_NULL_HANDLE);
	}

	void destroy(const DeviceContext& ctx) {
		ctx.lib.vkDestroySurfaceKHR(ctx.instance(), _surface, nullptr);
		_surface = VK_NULL_HANDLE;
	}

	VkSurfaceKHR get() const {
		return _surface;
	}

private:
	VkSurfaceKHR _surface;
};

} // namespace cltv
