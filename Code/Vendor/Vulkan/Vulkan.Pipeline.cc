#include "PCH.hh"

#include "Utils/Assert.hh"
#include "Vulkan.GPUContext.hh"
#include "Vulkan.Pipeline.hh"
#include "Vulkan.Utils.hh"

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

		vk::PipelineViewportStateCreateInfo viewportInfo;
		viewportInfo.viewportCount = 1;
		viewportInfo.scissorCount  = 1;

		auto rasterizerInfo = fillRasterizerInfo();

		vk::PipelineMultisampleStateCreateInfo multisamplerInfo;

		auto colorBlendAttachment = fillColorBlendAttachment();
		vk::PipelineColorBlendStateCreateInfo colorBlendInfo;
		colorBlendInfo.attachmentCount = 1;
		colorBlendInfo.pAttachments	   = &colorBlendAttachment;

		std::array dynamicStates {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
		vk::PipelineDynamicStateCreateInfo dynamicStateInfo;
		dynamicStateInfo.dynamicStateCount = count(dynamicStates);
		dynamicStateInfo.pDynamicStates	   = dynamicStates.data();

		vk::GraphicsPipelineCreateInfo pipelineInfo;
		pipelineInfo.stageCount			 = count(shaderStages);
		pipelineInfo.pStages			 = shaderStages.data();
		pipelineInfo.pVertexInputState	 = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &assemblyInfo;
		pipelineInfo.pViewportState		 = &viewportInfo;
		pipelineInfo.pRasterizationState = &rasterizerInfo;
		pipelineInfo.pMultisampleState	 = &multisamplerInfo;
		pipelineInfo.pColorBlendState	 = &colorBlendInfo;
		pipelineInfo.pDynamicState		 = &dynamicStateInfo;
		pipelineInfo.layout				 = layout.handle();
		pipelineInfo.renderPass			 = renderPass.handle();
		pipelineInfo.basePipelineIndex	 = -1;

		auto [res, pipe] = GPUContext::device().createGraphicsPipeline({}, pipelineInfo, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create Vulkan pipeline.");
		return pipe;
	}

	vk::PipelineShaderStageCreateInfo Pipeline::fillShaderStageInfo(vk::ShaderStageFlagBits stage, Shader const& shader)
	{
		vk::PipelineShaderStageCreateInfo info;
		info.stage	= stage;
		info.module = shader.handle();
		info.pName	= Shader::EntryPoint;
		return info;
	}

	vk::PipelineRasterizationStateCreateInfo Pipeline::fillRasterizerInfo()
	{
		vk::PipelineRasterizationStateCreateInfo info;
		info.cullMode  = vk::CullModeFlagBits::eFront;
		info.frontFace = vk::FrontFace::eCounterClockwise;
		info.lineWidth = 1.0f;
		return info;
	}

	vk::PipelineColorBlendAttachmentState Pipeline::fillColorBlendAttachment()
	{
		vk::PipelineColorBlendAttachmentState state;
		state.blendEnable	 = false;
		state.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
							   vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
		return state;
	}
}
