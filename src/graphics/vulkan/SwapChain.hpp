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
		return _framebuffers.at(image_index);
	}

	size_t get_image_count() const {
		return _images.size();
	}

private:
	const DeviceContext* _ctx;
	const Window* _window;
	Surface _surface;
	VkRenderPass _render_pass;
	VkSurfaceFormatKHR _surface_format;
	VkPresentModeKHR _present_mode;
	VkExtent2D _extent;
	VkSwapchainKHR _swapchain = VK_NULL_HANDLE;
	std::vector<VkImage> _images;
	std::vector<VkImageView> _image_views;
	std::vector<VkFramebuffer> _framebuffers;

	void recreate();
	void init_surface_format();
	void init_present_mode();
	void init_extent_and_swapchain(RectSize size);
	void init_images();
};

} // namespace cltv
