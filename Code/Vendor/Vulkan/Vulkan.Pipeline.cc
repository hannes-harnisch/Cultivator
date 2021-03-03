#include "PCH.hh"
#include "Vulkan.Pipeline.hh"

#include "Utils/Assert.hh"
#include "Vendor/Vulkan/Vulkan.GPUContext.hh"

namespace ct::vulkan
{
	Pipeline::Pipeline(const Shader& vertex, const Shader& fragment) : Layout {createLayout()}
	{
		std::array shaderStages {fillShaderStageInfo(vk::ShaderStageFlagBits::eVertex, vertex),
								 fillShaderStageInfo(vk::ShaderStageFlagBits::eFragment, fragment)};
		vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
		vk::PipelineInputAssemblyStateCreateInfo assemblyInfo({}, vk::PrimitiveTopology::eTriangleList);
		auto viewportInfo {vk::PipelineViewportStateCreateInfo().setViewportCount(1).setScissorCount(1)};
		auto rasterizerInfo {fillRasterizerInfo()};
		vk::PipelineMultisampleStateCreateInfo multisamplerInfo;

		std::array colorBlendAttachments {fillColorBlendAttachment()};
		auto colorBlendInfo {vk::PipelineColorBlendStateCreateInfo().setAttachments(colorBlendAttachments)};

		std::array dynamicStates {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
		auto dynamicStateInfo {vk::PipelineDynamicStateCreateInfo().setDynamicStates(dynamicStates)};

		RenderTarget renderTarget;
		auto pipelineInfo {vk::GraphicsPipelineCreateInfo()
							   .setStages(shaderStages)
							   .setPVertexInputState(&vertexInputInfo)
							   .setPInputAssemblyState(&assemblyInfo)
							   .setPViewportState(&viewportInfo)
							   .setPRasterizationState(&rasterizerInfo)
							   .setPMultisampleState(&multisamplerInfo)
							   .setPColorBlendState(&colorBlendInfo)
							   .setPDynamicState(&dynamicStateInfo)
							   .setLayout(Layout)
							   .setRenderPass(renderTarget.getRenderPass())
							   .setBasePipelineIndex(-1)};
		auto [res, pipe] {GPUContext::device().createGraphicsPipeline({}, pipelineInfo, nullptr, Loader::get())};
		ctAssertResult(res, "Failed to create Vulkan pipeline.");
		PipelineHandle = pipe;
	}

	Pipeline::~Pipeline()
	{
		auto device {GPUContext::device()};
		device.destroyPipeline(PipelineHandle, {}, Loader::get());
		device.destroyPipelineLayout(Layout, {}, Loader::get());
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

	vk::PipelineLayout Pipeline::createLayout()
	{
		vk::PipelineLayoutCreateInfo layoutInfo;
		auto [res, layout] {GPUContext::device().createPipelineLayout(layoutInfo, nullptr, Loader::get())};
		ctAssertResult(res, "Failed to create Vulkan pipeline layout.");
		return layout;
	}

	vk::PipelineShaderStageCreateInfo Pipeline::fillShaderStageInfo(vk::ShaderStageFlagBits stage, const Shader& shader)
	{
		return vk::PipelineShaderStageCreateInfo()
			.setStage(stage)
			.setModule(shader.handle())
			.setPName(Shader::EntryPoint);
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
