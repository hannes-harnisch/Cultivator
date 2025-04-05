#pragma once

#include "app/Window.hpp"
#include "graphics/vulkan/DeviceContext.hpp"
#include "graphics/vulkan/texture.hpp"
#include "util/RectSize.hpp"

namespace cltv {

class AutomatonRenderer {
public:
	AutomatonRenderer(Window& window, RectSize size);
	~AutomatonRenderer();

	void draw_frame();

private:
	DeviceContext _ctx;
	RectSize _window_size;
	Texture _front_texture;
	Texture _back_texture;
};

} // namespace cltv
