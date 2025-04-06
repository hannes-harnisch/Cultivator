#include "AutomatonRenderer.hpp"

namespace cltv {

AutomatonRenderer::AutomatonRenderer(const DeviceContext* ctx, Window& window, RectSize size) :
	_ctx(ctx),
	_window_size(window.get_viewport()),
	_simulation_pass(*_ctx, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL),
	_presentation_pass(*_ctx, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR),
	_swap_chain(*_ctx, _window_size, window, _presentation_pass),
	_front_target(*_ctx, size, _simulation_pass),
	_back_target(*_ctx, size, _simulation_pass) {
}

AutomatonRenderer::~AutomatonRenderer() {
	_back_target.destroy(*_ctx);
	_front_target.destroy(*_ctx);
	_swap_chain.destroy(*_ctx);
	_presentation_pass.destroy(*_ctx);
	_simulation_pass.destroy(*_ctx);
}

void AutomatonRenderer::draw_frame() {
}

} // namespace cltv
