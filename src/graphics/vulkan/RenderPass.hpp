#pragma once

#include "graphics/vulkan/DeviceContext.hpp"

namespace cltv {

class RenderPass {
public:
	RenderPass(const DeviceContext* ctx, VkImageLayout initial, VkImageLayout final);
	~RenderPass();

	VkRenderPass get() const {
		return render_pass_;
	}

private:
	const DeviceContext* ctx_;
	VkRenderPass render_pass_ = VK_NULL_HANDLE;
};

} // namespace cltv
