#pragma once

#include "PCH.hh"

namespace ct::vulkan
{
	class RenderTarget final
	{
	public:
		RenderTarget();
		~RenderTarget();
		RenderTarget(RenderTarget&& other) noexcept;
		RenderTarget& operator=(RenderTarget&& other) noexcept;

		inline vk::RenderPass getRenderPass() const
		{
			return RenderPass;
		}

		inline vk::Framebuffer getFrameBuffer() const
		{
			return FrameBuffer;
		}

	private:
		vk::RenderPass RenderPass;
		vk::Framebuffer FrameBuffer;

		vk::AttachmentDescription fillAttachmentDescription(vk::ImageLayout initial, vk::ImageLayout final);
	};
}
