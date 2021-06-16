#include "PCH.hh"

#include "Vulkan.GPUContext.hh"
#include "Vulkan.PipelineLayout.hh"
#include "Vulkan.Utils.hh"

namespace ct
{
	PipelineLayout::PipelineLayout(std::vector<vk::DescriptorSetLayout> const& descriptorLayouts)
	{
		vk::PipelineLayoutCreateInfo info;
		info.setLayoutCount = count(descriptorLayouts);
		info.pSetLayouts	= descriptorLayouts.data();

		auto [res, layout] = GPUContext::device().createPipelineLayout(info);
		ctAssertResult(res, "Failed to create Vulkan pipeline layout.");
		pipelineLayout = layout;
	}
}
