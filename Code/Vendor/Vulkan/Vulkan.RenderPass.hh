#pragma once

#include "PCH.hh"

#include "Vulkan.Unique.hh"

namespace ct
{
	class RenderPass final
	{
	public:
		RenderPass(vk::ImageLayout initial, vk::ImageLayout final);

		vk::RenderPass handle() const
		{
			return renderPass;
		}

	private:
		DeviceOwn<vk::RenderPass, &vk::Device::destroyRenderPass> renderPass;
	};
}
