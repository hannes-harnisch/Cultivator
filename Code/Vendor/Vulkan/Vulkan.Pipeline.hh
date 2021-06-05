#pragma once

#include "PCH.hh"

#include "Vendor/Vulkan/Vulkan.PipelineLayout.hh"
#include "Vendor/Vulkan/Vulkan.Shader.hh"
#include "Vendor/Vulkan/Vulkan.Unique.hh"

namespace ct
{
	class Pipeline final
	{
	public:
		Pipeline(Shader const& vertex, Shader const& fragment, PipelineLayout const& layout, RenderPass const& renderPass);

		vk::Pipeline handle() const
		{
			return pipeline;
		}

	private:
		DeviceUnique<vk::Pipeline, &vk::Device::destroyPipeline> pipeline;

		vk::Pipeline createPipeline(Shader const& vertex,
									Shader const& fragment,
									PipelineLayout const& layout,
									RenderPass const& renderPass);
		vk::PipelineShaderStageCreateInfo fillShaderStageInfo(vk::ShaderStageFlagBits stage, Shader const& shader);
		vk::PipelineRasterizationStateCreateInfo fillRasterizerInfo();
		vk::PipelineColorBlendAttachmentState fillColorBlendAttachment();
	};
}
