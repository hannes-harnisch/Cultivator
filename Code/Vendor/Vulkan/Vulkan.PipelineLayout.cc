#include "PCH.hh"

#include "Vulkan.GPUContext.hh"
#include "Vulkan.PipelineLayout.hh"

namespace ct
{
	PipelineLayout::PipelineLayout(std::vector<vk::DescriptorSetLayout> const& descriptorLayouts) :
		pipelineLayout(makePipelineLayout(descriptorLayouts))
	{}

	vk::PipelineLayout PipelineLayout::makePipelineLayout(std::vector<vk::DescriptorSetLayout> const& descriptorLayouts)
	{
		auto layoutInfo	   = vk::PipelineLayoutCreateInfo().setSetLayouts(descriptorLayouts);
		auto [res, layout] = GPUContext::device().createPipelineLayout(layoutInfo, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create Vulkan pipeline layout.");
		return layout;
	}
}
