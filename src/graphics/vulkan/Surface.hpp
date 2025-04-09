#pragma once

#include "app/Window.hpp"
#include "graphics/vulkan/DeviceContext.hpp"

namespace cltv {

class Surface {
public:
	Surface(const DeviceContext* ctx, const Window& window);

	~Surface() {
		_ctx->lib.vkDestroySurfaceKHR(_ctx->instance(), _surface, nullptr);
	}

	VkSurfaceKHR get() const {
		return _surface;
	}

private:
	const DeviceContext* _ctx;
	VkSurfaceKHR _surface;
};

} // namespace cltv
