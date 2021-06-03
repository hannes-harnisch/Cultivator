#pragma once

#include "PCH.hh"

#include "Vendor/Vulkan/Vulkan.Unique.hh"

namespace ct::vulkan
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
		DeviceUnique<vk::PipelineLayout, &vk::Device::destroyPipelineLayout> pipelineLayout;

		static vk::PipelineLayout makePipelineLayout(std::vector<vk::DescriptorSetLayout> const& descriptorLayouts);
	};
}
