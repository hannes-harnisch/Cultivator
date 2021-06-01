#pragma once

#include "PCH.hh"

#include "Utils/Rectangle.hh"
#include "Vendor/Vulkan/Vulkan.RenderPass.hh"
#include "Vendor/Vulkan/Vulkan.Unique.hh"

namespace ct::vulkan
{
	class FrameBuffer final
	{
	public:
		FrameBuffer(Rectangle size, const RenderPass& renderPass);

		vk::Framebuffer handle() const
		{
			return FrameBufferHandle;
		}

	private:
		DeviceUnique<vk::Framebuffer, &vk::Device::destroyFramebuffer> FrameBufferHandle;

		static vk::Framebuffer createFrameBuffer(Rectangle size, const RenderPass& renderPass);
	};
}
