#include "PCH.hh"
#include "Vulkan.Pipeline.hh"

#include "Vendor/Vulkan/Vulkan.GraphicsContext.hh"

namespace ct::vulkan
{
	Pipeline::~Pipeline()
	{
		GraphicsContext::device().destroyPipeline(PipelineHandle);
	}

	Pipeline::Pipeline(Pipeline&& other) noexcept : PipelineHandle {std::exchange(other.PipelineHandle, nullptr)}
	{}

	Pipeline& Pipeline::operator=(Pipeline&& other) noexcept
	{
		std::swap(PipelineHandle, other.PipelineHandle);
		return *this;
	}
}
