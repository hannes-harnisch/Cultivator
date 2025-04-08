#pragma once

#include "graphics/vulkan/DeviceContext.hpp"
#include "graphics/vulkan/RenderPass.hpp"

namespace cltv {

class RenderTarget {
public:
	RenderTarget(const DeviceContext* ctx, RectSize size, const RenderPass& render_pass);
	~RenderTarget();

	RectSize get_size() const {
		return _size;
	}

	VkImageView get_image_view() const {
		return _image_view;
	}

private:
	const DeviceContext* _ctx;
	RectSize _size;
	VkDeviceMemory _memory	   = VK_NULL_HANDLE;
	VkImage _image			   = VK_NULL_HANDLE;
	VkImageView _image_view	   = VK_NULL_HANDLE;
	VkFramebuffer _framebuffer = VK_NULL_HANDLE;

	void init_image(RectSize size);
	void init_memory();
	void init_view_and_framebuffer(RectSize size, const RenderPass& render_pass);
};

} // namespace cltv
