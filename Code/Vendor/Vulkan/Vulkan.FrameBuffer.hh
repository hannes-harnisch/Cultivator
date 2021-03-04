#pragma once

#include "PCH.hh"
#include "Utils/Rectangle.hh"
#include "Vendor/Vulkan/Vulkan.RenderPass.hh"

namespace ct::vulkan
{
	class FrameBuffer final
	{
	public:
		FrameBuffer(Rectangle size, const RenderPass& renderPass);
		FrameBuffer(FrameBuffer&& other) noexcept;
		~FrameBuffer();
		FrameBuffer& operator=(FrameBuffer&& other) noexcept;

		inline vk::Framebuffer handle() const
		{
			return FrameBufferHandle;
		}

	private:
		vk::Framebuffer FrameBufferHandle;
	};
}
