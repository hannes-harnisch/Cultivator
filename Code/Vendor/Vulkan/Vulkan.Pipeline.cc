#include "PCH.hh"
#include "Vulkan.Pipeline.hh"

#include "Vendor/Vulkan/Vulkan.GraphicsContext.hh"

namespace ct::vulkan
{
	Pipeline::Pipeline(const Shader& vertex, const Shader& fragment)
	{
		auto shaderStages {fillShaderStages(vertex, fragment)};
		vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
		auto rasterizerInfo {fillRasterizerInfo()};
	}

	Pipeline::~Pipeline()
	{
		GraphicsContext::device().destroyPipeline(PipelineHandle);
		GraphicsContext::device().destroyPipelineLayout(Layout);
	}

	Pipeline::Pipeline(Pipeline&& other) noexcept :
		PipelineHandle {std::exchange(other.PipelineHandle, nullptr)}, Layout {std::exchange(other.Layout, nullptr)}
	{}

	Pipeline& Pipeline::operator=(Pipeline&& other) noexcept
	{
		std::swap(PipelineHandle, other.PipelineHandle);
		std::swap(Layout, other.Layout);
		return *this;
	}

	std::array<vk::PipelineShaderStageCreateInfo, 2> Pipeline::fillShaderStages(const Shader& vertex,
																				const Shader& fragment)
	{
		auto vertexInfo {vk::PipelineShaderStageCreateInfo()
							 .setStage(vk::ShaderStageFlagBits::eVertex)
							 .setModule(vertex.handle())
							 .setPName("main")};
		auto fragmentInfo {vk::PipelineShaderStageCreateInfo()
							   .setStage(vk::ShaderStageFlagBits::eFragment)
							   .setModule(fragment.handle())
							   .setPName("main")};
		return {vertexInfo, fragmentInfo};
	}

	vk::PipelineRasterizationStateCreateInfo Pipeline::fillRasterizerInfo()
	{
		return vk::PipelineRasterizationStateCreateInfo()
			.setCullMode(vk::CullModeFlagBits::eFront)
			.setFrontFace(vk::FrontFace::eCounterClockwise)
			.setLineWidth(1.0f);
	}
}
