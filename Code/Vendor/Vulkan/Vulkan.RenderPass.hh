#pragma once

#include "PCH.hh"

namespace ct::vulkan
{
	class RenderPass final
	{
	public:
		RenderPass();
		~RenderPass();
		RenderPass(RenderPass&& other) noexcept;
		RenderPass& operator=(RenderPass&& other) noexcept;

		inline vk::RenderPass handle() const
		{
			return RenderPassHandle;
		}

	private:
		vk::RenderPass RenderPassHandle;

		vk::AttachmentDescription fillAttachmentDescription(vk::ImageLayout initial, vk::ImageLayout final);
	};
}
