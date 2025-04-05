#include "AutomatonRenderer.hpp"

namespace cltv {

#ifdef NDEBUG
const bool EnableDebugLayer = false;
#else
const bool EnableDebugLayer = true;
#endif

AutomatonRenderer::AutomatonRenderer(Window& window, RectSize size) :
	_ctx(window, EnableDebugLayer),
	_window_size(window.get_viewport()),
	_front_texture(_ctx, size),
	_back_texture(_ctx, size) {
}

AutomatonRenderer::~AutomatonRenderer() {
	_back_texture.destroy(_ctx);
	_front_texture.destroy(_ctx);
}

void AutomatonRenderer::draw_frame() {
}

} // namespace cltv
