#pragma once

namespace cltv {

struct RectSize {
	int32_t width  = 0;
	int32_t height = 0;

	int32_t area() const {
		return width * height;
	}

	bool is_empty() const {
		return width == 0 || height == 0;
	}
};

} // namespace cltv
