#pragma once

#include "graphics/vulkan/DeviceContext.hpp"

namespace cltv {

class RenderPass {
public:
	RenderPass(const DeviceContext* ctx, VkImageLayout initial, VkImageLayout final);
	~RenderPass();

	VkRenderPass get() const {
		return _render_pass;
	}

private:
	const DeviceContext* _ctx;
	VkRenderPass _render_pass = VK_NULL_HANDLE;
};

} // namespace cltv
