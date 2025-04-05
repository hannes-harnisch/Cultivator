#pragma once

#include "graphics/vulkan/DeviceContext.hpp"

namespace cltv {

class Texture {
public:
	Texture(const DeviceContext& ctx, RectSize size);
	~Texture();

	void destroy(const DeviceContext& ctx);

	VkImage get_image() const {
		return _image;
	}

	VkImageView get_view() const {
		return _image_view;
	}

private:
	RectSize _size;
	VkDeviceMemory _memory	= VK_NULL_HANDLE;
	VkImage _image			= VK_NULL_HANDLE;
	VkImageView _image_view = VK_NULL_HANDLE;
};

} // namespace cltv
