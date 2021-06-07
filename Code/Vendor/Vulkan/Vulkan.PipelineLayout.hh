#pragma once

#include "PCH.hh"

#include "Vulkan.Unique.hh"

namespace ct
{
	class PipelineLayout final
	{
	public:
		PipelineLayout(std::vector<vk::DescriptorSetLayout> const& descriptorLayouts);

		vk::PipelineLayout handle() const
		{
			return pipelineLayout;
		}

	private:
		DeviceOwn<vk::PipelineLayout, &vk::Device::destroyPipelineLayout> pipelineLayout;
	};
}
