#include "PCH.hh"

#include "Utils/Assert.hh"
#include "Vendor/Vulkan/Vulkan.GPUContext.hh"
#include "Vendor/Vulkan/Vulkan.RenderPass.hh"
#include "Vulkan.Pipeline.hh"

namespace ct
{
	Pipeline::Pipeline(Shader const& vertex,
					   Shader const& fragment,
					   PipelineLayout const& layout,
					   RenderPass const& renderPass) :
		pipeline(createPipeline(vertex, fragment, layout, renderPass))
	{}

	vk::Pipeline Pipeline::createPipeline(Shader const& vertex,
										  Shader const& fragment,
										  PipelineLayout const& layout,
										  RenderPass const& renderPass)
	{
		std::array shaderStages {fillShaderStageInfo(vk::ShaderStageFlagBits::eVertex, vertex),
								 fillShaderStageInfo(vk::ShaderStageFlagBits::eFragment, fragment)};
		vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
		vk::PipelineInputAssemblyStateCreateInfo assemblyInfo({}, vk::PrimitiveTopology::eTriangleList);
		auto viewportInfo	= vk::PipelineViewportStateCreateInfo().setViewportCount(1).setScissorCount(1);
		auto rasterizerInfo = fillRasterizerInfo();
		vk::PipelineMultisampleStateCreateInfo multisamplerInfo;

		std::array colorBlendAttachments {fillColorBlendAttachment()};
		auto colorBlendInfo = vk::PipelineColorBlendStateCreateInfo().setAttachments(colorBlendAttachments);

		std::array dynamicStates {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
		auto dynamicStateInfo = vk::PipelineDynamicStateCreateInfo().setDynamicStates(dynamicStates);

		auto pipelineInfo = vk::GraphicsPipelineCreateInfo()
								.setStages(shaderStages)
								.setPVertexInputState(&vertexInputInfo)
								.setPInputAssemblyState(&assemblyInfo)
								.setPViewportState(&viewportInfo)
								.setPRasterizationState(&rasterizerInfo)
								.setPMultisampleState(&multisamplerInfo)
								.setPColorBlendState(&colorBlendInfo)
								.setPDynamicState(&dynamicStateInfo)
								.setLayout(layout.handle())
								.setRenderPass(renderPass.handle())
								.setBasePipelineIndex(-1);
		auto [res, pipe] = GPUContext::device().createGraphicsPipeline({}, pipelineInfo, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create Vulkan pipeline.");
		return pipe;
	}

	vk::PipelineShaderStageCreateInfo Pipeline::fillShaderStageInfo(vk::ShaderStageFlagBits stage, Shader const& shader)
	{
		return vk::PipelineShaderStageCreateInfo().setStage(stage).setModule(shader.handle()).setPName(Shader::EntryPoint);
	}

	vk::PipelineRasterizationStateCreateInfo Pipeline::fillRasterizerInfo()
	{
		return vk::PipelineRasterizationStateCreateInfo()
			.setCullMode(vk::CullModeFlagBits::eFront)
			.setFrontFace(vk::FrontFace::eCounterClockwise)
			.setLineWidth(1.0f);
	}

	vk::PipelineColorBlendAttachmentState Pipeline::fillColorBlendAttachment()
	{
		return vk::PipelineColorBlendAttachmentState()
			.setBlendEnable(true)
			.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
			.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
			.setColorBlendOp(vk::BlendOp::eAdd)
			.setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
			.setDstAlphaBlendFactor(vk::BlendFactor::eZero)
			.setAlphaBlendOp(vk::BlendOp::eAdd)
			.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
							   vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
	}
}
