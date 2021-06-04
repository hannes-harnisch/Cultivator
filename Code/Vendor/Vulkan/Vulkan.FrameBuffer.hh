#pragma once

#include "PCH.hh"

#include "Utils/Rectangle.hh"
#include "Vendor/Vulkan/Vulkan.RenderPass.hh"
#include "Vendor/Vulkan/Vulkan.Unique.hh"

namespace ct
{
	class FrameBuffer final
	{
	public:
		FrameBuffer(Rectangle size, RenderPass const& renderPass);

		vk::Framebuffer handle() const
		{
			return frameBuffer;
		}

	private:
		DeviceUnique<vk::Framebuffer, &vk::Device::destroyFramebuffer> frameBuffer;

		static vk::Framebuffer createFrameBuffer(Rectangle size, RenderPass const& renderPass);
	};
}
