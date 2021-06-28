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
		vk::PipelineInputAssemblyStateCreateInfo assemblyInfo {
			.topology = vk::PrimitiveTopology::eTriangleList,
		};

		vk::PipelineViewportStateCreateInfo viewportInfo {
			.viewportCount = 1,
			.scissorCount  = 1,
		};
		vk::PipelineRasterizationStateCreateInfo rasterizerInfo {
			.cullMode  = vk::CullModeFlagBits::eFront,
			.frontFace = vk::FrontFace::eCounterClockwise,
			.lineWidth = 1.0f,
		};
		vk::PipelineMultisampleStateCreateInfo multisamplerInfo;

		auto colorBlendAttachment = fillColorBlendAttachment();
		vk::PipelineColorBlendStateCreateInfo colorBlendInfo {
			.attachmentCount = 1,
			.pAttachments	 = &colorBlendAttachment,
		};

		std::array dynamicStates {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
		vk::PipelineDynamicStateCreateInfo dynamicStateInfo {
			.dynamicStateCount = count(dynamicStates),
			.pDynamicStates	   = dynamicStates.data(),
		};

		vk::GraphicsPipelineCreateInfo pipelineInfo {
			.stageCount			 = count(shaderStages),
			.pStages			 = shaderStages.data(),
			.pVertexInputState	 = &vertexInputInfo,
			.pInputAssemblyState = &assemblyInfo,
			.pViewportState		 = &viewportInfo,
			.pRasterizationState = &rasterizerInfo,
			.pMultisampleState	 = &multisamplerInfo,
			.pColorBlendState	 = &colorBlendInfo,
			.pDynamicState		 = &dynamicStateInfo,
			.layout				 = layout.handle(),
			.renderPass			 = renderPass.handle(),
			.basePipelineIndex	 = -1,
		};
		auto [res, pipe] = GPUContext::device().createGraphicsPipeline({}, pipelineInfo);
		ctAssertResult(res, "Failed to create Vulkan pipeline.");
		return pipe;
	}

	vk::PipelineShaderStageCreateInfo Pipeline::fillShaderStageInfo(vk::ShaderStageFlagBits stage, Shader const& shader)
	{
		return vk::PipelineShaderStageCreateInfo {
			.stage	= stage,
			.module = shader.handle(),
			.pName	= Shader::EntryPoint,
		};
	}

	vk::PipelineColorBlendAttachmentState Pipeline::fillColorBlendAttachment()
	{
		vk::PipelineColorBlendAttachmentState state {
			.blendEnable	= false,
			.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB |
							  vk::ColorComponentFlagBits::eA,
		};
		return state;
	}
}
