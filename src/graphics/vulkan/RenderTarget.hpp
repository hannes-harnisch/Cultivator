#pragma once

#include "graphics/vulkan/DeviceContext.hpp"
#include "graphics/vulkan/RenderPass.hpp"

namespace cltv {

class RenderTarget {
public:
	RenderTarget(const DeviceContext& ctx, RectSize size, const RenderPass& render_pass);
	~RenderTarget();

	void destroy(const DeviceContext& ctx);

	VkImageView get_image_view() const {
		return _image_view;
	}

private:
	RectSize _size;
	VkDeviceMemory _memory	   = VK_NULL_HANDLE;
	VkImage _image			   = VK_NULL_HANDLE;
	VkImageView _image_view	   = VK_NULL_HANDLE;
	VkFramebuffer _framebuffer = VK_NULL_HANDLE;

	void init_image(const DeviceContext& ctx, RectSize size);
	void init_memory(const DeviceContext& ctx);
	void init_view_and_framebuffer(const DeviceContext& ctx, RectSize size, const RenderPass& render_pass);
};

} // namespace cltv
