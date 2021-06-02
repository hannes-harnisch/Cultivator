#pragma once

#include "PCH.hh"

#include "Vendor/Vulkan/Vulkan.Unique.hh"

namespace ct::vulkan
{
	class PipelineLayout final
	{
	public:
		PipelineLayout();

		vk::PipelineLayout handle() const
		{
			return Layout;
		}

	private:
		DeviceUnique<vk::PipelineLayout, &vk::Device::destroyPipelineLayout> Layout;

		static vk::PipelineLayout createPipelineLayout();
	};
}
