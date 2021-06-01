#pragma once

#include "PCH.hh"

#include "Vendor/Vulkan/Vulkan.Unique.hh"

namespace ct::vulkan
{
	class RenderPass final
	{
	public:
		RenderPass();

		vk::RenderPass handle() const
		{
			return RenderPassHandle;
		}

	private:
		DeviceUnique<vk::RenderPass, &vk::Device::destroyRenderPass> RenderPassHandle;

		static vk::RenderPass createRenderPass();
		static vk::AttachmentDescription fillAttachmentDescription(vk::ImageLayout initial, vk::ImageLayout final);
	};
}
