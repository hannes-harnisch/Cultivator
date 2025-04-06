#pragma once

#include "app/Window.hpp"
#include "graphics/vulkan/DeviceContext.hpp"
#include "graphics/vulkan/RenderPass.hpp"
#include "graphics/vulkan/Surface.hpp"

namespace cltv {

class SwapChain {
public:
	SwapChain(const DeviceContext& ctx, RectSize size, Window& window, const RenderPass& render_pass);
	~SwapChain();

	void destroy(const DeviceContext& ctx);

private:
	Surface _surface;
	VkSurfaceFormatKHR _surface_format;
	VkPresentModeKHR _present_mode;
	VkExtent2D _extent;
	VkSwapchainKHR _swapchain = VK_NULL_HANDLE;
	std::vector<VkImage> _images;
	std::vector<VkImageView> _image_views;
	std::vector<VkFramebuffer> _framebuffers;

	void init_surface_format(const DeviceContext& ctx);
	void init_present_mode(const DeviceContext& ctx);
	void init_extent_and_swapchain(const DeviceContext& ctx, RectSize size);
	void init_images(const DeviceContext& ctx, const RenderPass& render_pass);
};

} // namespace cltv
