#include "Pipeline.hpp"

#include "graphics/vulkan/Util.hpp"

namespace cltv {

Pipeline::Pipeline(const DeviceContext* ctx,
				   VkShaderModule vertex,
				   VkShaderModule fragment,
				   VkPipelineLayout layout,
				   const RenderPass& render_pass) :
	_ctx(ctx) {
	const VkPipelineShaderStageCreateInfo shader_stages[] = {
		VkPipelineShaderStageCreateInfo {
			.sType				 = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext				 = nullptr,
			.flags				 = 0,
			.stage				 = VK_SHADER_STAGE_VERTEX_BIT,
			.module				 = vertex,
			.pName				 = "main",
			.pSpecializationInfo = nullptr,
		},
		VkPipelineShaderStageCreateInfo {
			.sType				 = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext				 = nullptr,
			.flags				 = 0,
			.stage				 = VK_SHADER_STAGE_FRAGMENT_BIT,
			.module				 = fragment,
			.pName				 = "main",
			.pSpecializationInfo = nullptr,
		},
	};
	VkPipelineVertexInputStateCreateInfo vertex_input_state {
		.sType							 = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.pNext							 = nullptr,
		.flags							 = 0,
		.vertexBindingDescriptionCount	 = 0,
		.pVertexBindingDescriptions		 = nullptr,
		.vertexAttributeDescriptionCount = 0,
		.pVertexAttributeDescriptions	 = nullptr,
	};
	VkPipelineInputAssemblyStateCreateInfo input_assembly_state {
		.sType					= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.pNext					= nullptr,
		.flags					= 0,
		.topology				= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE,
	};
	VkPipelineViewportStateCreateInfo viewport_state {
		.sType		   = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.pNext		   = nullptr,
		.flags		   = 0,
		.viewportCount = 1,
		.pViewports	   = nullptr,
		.scissorCount  = 1,
		.pScissors	   = nullptr,
	};
	VkPipelineRasterizationStateCreateInfo rasterization_state {
		.sType					 = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.pNext					 = nullptr,
		.flags					 = 0,
		.depthClampEnable		 = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode			 = VK_POLYGON_MODE_FILL,
		.cullMode				 = VK_CULL_MODE_FRONT_BIT,
		.frontFace				 = VK_FRONT_FACE_COUNTER_CLOCKWISE,
		.depthBiasEnable		 = VK_FALSE,
		.depthBiasConstantFactor = 0,
		.depthBiasClamp			 = 0,
		.depthBiasSlopeFactor	 = 0,
		.lineWidth				 = 1.0f,
	};
	VkPipelineMultisampleStateCreateInfo multisample_state {
		.sType				   = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.pNext				   = nullptr,
		.flags				   = 0,
		.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable   = VK_FALSE,
		.minSampleShading	   = 0,
		.pSampleMask		   = nullptr,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable	   = VK_FALSE,
	};
	VkPipelineColorBlendAttachmentState attachment {
		.blendEnable		 = VK_FALSE,
		.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO,
		.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
		.colorBlendOp		 = VK_BLEND_OP_ADD,
		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		.alphaBlendOp		 = VK_BLEND_OP_ADD,
		.colorWriteMask		 = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
						  | VK_COLOR_COMPONENT_A_BIT,
	};
	VkPipelineColorBlendStateCreateInfo color_blend_state {
		.sType			 = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.pNext			 = nullptr,
		.flags			 = 0,
		.logicOpEnable	 = VK_FALSE,
		.logicOp		 = VK_LOGIC_OP_CLEAR,
		.attachmentCount = 1,
		.pAttachments	 = &attachment,
		.blendConstants	 = {0, 0, 0, 0},
	};
	const VkDynamicState dynamic_states[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
	};
	VkPipelineDynamicStateCreateInfo dynamic_state {
		.sType			   = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.pNext			   = nullptr,
		.flags			   = 0,
		.dynamicStateCount = get_count(dynamic_states),
		.pDynamicStates	   = dynamic_states,
	};

	VkGraphicsPipelineCreateInfo pipeline_info {
		.sType				 = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext				 = nullptr,
		.flags				 = 0,
		.stageCount			 = get_count(shader_stages),
		.pStages			 = shader_stages,
		.pVertexInputState	 = &vertex_input_state,
		.pInputAssemblyState = &input_assembly_state,
		.pTessellationState	 = nullptr,
		.pViewportState		 = &viewport_state,
		.pRasterizationState = &rasterization_state,
		.pMultisampleState	 = &multisample_state,
		.pDepthStencilState	 = nullptr,
		.pColorBlendState	 = &color_blend_state,
		.pDynamicState		 = &dynamic_state,
		.layout				 = layout,
		.renderPass			 = render_pass.get(),
		.subpass			 = 0,
		.basePipelineHandle	 = VK_NULL_HANDLE,
		.basePipelineIndex	 = -1,
	};
	VkResult result = ctx->lib.vkCreateGraphicsPipelines(ctx->device(), VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &_pipeline);
	require_vk_result(result, "failed to create Vulkan pipeline");
}

Pipeline::~Pipeline() {
	_ctx->lib.vkDestroyPipeline(_ctx->device(), _pipeline, nullptr);
}

} // namespace cltv
