#include "PCH.hh"

#include "Vulkan.GPUContext.hh"
#include "Vulkan.PipelineLayout.hh"

namespace ct::vulkan
{
	PipelineLayout::PipelineLayout() : Layout(createPipelineLayout())
	{}

	vk::PipelineLayout PipelineLayout::createPipelineLayout()
	{
		vk::PipelineLayoutCreateInfo layoutInfo;
		auto [res, layout] = GPUContext::device().createPipelineLayout(layoutInfo, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create Vulkan pipeline layout.");
		return layout;
	}
}
