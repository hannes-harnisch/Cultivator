#pragma once

#include "app/Window.hpp"
#include "graphics/vulkan/DeviceContext.hpp"
#include "graphics/vulkan/RenderPass.hpp"
#include "graphics/vulkan/RenderTarget.hpp"
#include "graphics/vulkan/SwapChain.hpp"
#include "util/RectSize.hpp"

namespace cltv {

class AutomatonRenderer {
public:
	AutomatonRenderer(const DeviceContext* ctx, Window& window, RectSize size);
	~AutomatonRenderer();

	void draw_frame();

private:
	const DeviceContext* _ctx;
	RectSize _window_size;
	RenderPass _simulation_pass;
	RenderPass _presentation_pass;
	SwapChain _swap_chain;
	RenderTarget _front_target;
	RenderTarget _back_target;
};

} // namespace cltv
