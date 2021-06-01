#pragma once

#include "PCH.hh"

#include "Vendor/Vulkan/Vulkan.Shader.hh"
#include "Vendor/Vulkan/Vulkan.Unique.hh"

namespace ct::vulkan
{
	class Pipeline final
	{
	public:
		Pipeline(const Shader& vertex, const Shader& fragment);

		vk::Pipeline handle() const
		{
			return PipelineHandle;
		}

		vk::PipelineLayout layout() const
		{
			return Layout;
		}

	private:
		DeviceUnique<vk::PipelineLayout, &vk::Device::destroyPipelineLayout> Layout;
		DeviceUnique<vk::Pipeline, &vk::Device::destroyPipeline> PipelineHandle;

		vk::PipelineLayout createLayout();
		vk::Pipeline createPipeline(const Shader& vertex, const Shader& fragment);
		vk::PipelineShaderStageCreateInfo fillShaderStageInfo(vk::ShaderStageFlagBits stage, const Shader& shader);
		vk::PipelineRasterizationStateCreateInfo fillRasterizerInfo();
		vk::PipelineColorBlendAttachmentState fillColorBlendAttachment();
	};
}
