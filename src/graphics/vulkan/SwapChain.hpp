#pragma once

#include "app/Window.hpp"
#include "graphics/vulkan/DeviceContext.hpp"
#include "graphics/vulkan/RenderPass.hpp"
#include "graphics/vulkan/Surface.hpp"

namespace cltv {

class SwapChain {
public:
	SwapChain(const DeviceContext* ctx, const Window* window, const RenderPass& render_pass);
	~SwapChain();

	RectSize get_size() const;

	std::optional<uint32_t> get_next_image_index(VkSemaphore img_acquire_semaphore);

	void present(uint32_t image_index, VkSemaphore img_release_semaphore);

	VkFramebuffer get_framebuffer(uint32_t image_index) const {
		return framebuffers_.at(image_index);
	}

	size_t get_image_count() const {
		return images_.size();
	}

private:
	const DeviceContext* ctx_;
	const Window* window_;
	Surface surface_;
	VkRenderPass render_pass_;
	VkSurfaceFormatKHR surface_format_;
	VkPresentModeKHR present_mode_;
	VkExtent2D extent_;
	VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
	std::vector<VkImage> images_;
	std::vector<VkImageView> image_views_;
	std::vector<VkFramebuffer> framebuffers_;

	void recreate();
	void init_surface_format();
	void init_present_mode();
	void init_extent_and_swapchain(RectSize size);
	void init_images();
};

} // namespace cltv
