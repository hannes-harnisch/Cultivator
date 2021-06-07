#pragma once

#include "PCH.hh"

#include "Utils/Rectangle.hh"
#include "Vulkan.RenderPass.hh"
#include "Vulkan.Unique.hh"

namespace ct
{
	class FrameBuffer final
	{
	public:
		FrameBuffer(Rectangle size, RenderPass const& renderPass, vk::ImageView attachment);

		vk::Framebuffer handle() const
		{
			return frameBuffer;
		}

	private:
		DeviceOwn<vk::Framebuffer, &vk::Device::destroyFramebuffer> frameBuffer;
	};
}
