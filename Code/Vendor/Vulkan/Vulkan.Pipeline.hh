#pragma once

#include "PCH.hh"

#include "Vendor/Vulkan/Vulkan.PipelineLayout.hh"
#include "Vendor/Vulkan/Vulkan.Shader.hh"
#include "Vendor/Vulkan/Vulkan.Unique.hh"

namespace ct::vulkan
{
	class Pipeline final
	{
	public:
		Pipeline(Shader const& vertex, Shader const& fragment, PipelineLayout const& layout);

		vk::Pipeline handle() const
		{
			return pipeline;
		}

	private:
		DeviceUnique<vk::Pipeline, &vk::Device::destroyPipeline> pipeline;

		vk::Pipeline createPipeline(Shader const& vertex, Shader const& fragment, PipelineLayout const& layout);
		vk::PipelineShaderStageCreateInfo fillShaderStageInfo(vk::ShaderStageFlagBits stage, Shader const& shader);
		vk::PipelineRasterizationStateCreateInfo fillRasterizerInfo();
		vk::PipelineColorBlendAttachmentState fillColorBlendAttachment();
	};
}
