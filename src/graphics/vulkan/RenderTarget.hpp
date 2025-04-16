#pragma once

#include "graphics/vulkan/DeviceContext.hpp"
#include "graphics/vulkan/RenderPass.hpp"

namespace cltv {

class RenderTarget {
public:
	RenderTarget(const DeviceContext* ctx, RectSize size, const RenderPass& render_pass);
	~RenderTarget();

	RectSize get_size() const {
		return size_;
	}

	VkImage get_image() const {
		return image_;
	}

	VkImageView get_image_view() const {
		return image_view_;
	}

	VkFramebuffer get_framebuffer() const {
		return framebuffer_;
	}

private:
	const DeviceContext* ctx_;
	RectSize size_;
	VkDeviceMemory memory_	   = VK_NULL_HANDLE;
	VkImage image_			   = VK_NULL_HANDLE;
	VkImageView image_view_	   = VK_NULL_HANDLE;
	VkFramebuffer framebuffer_ = VK_NULL_HANDLE;

	void init_image(RectSize size);
	void init_memory();
	void init_view_and_framebuffer(RectSize size, const RenderPass& render_pass);
};

} // namespace cltv
