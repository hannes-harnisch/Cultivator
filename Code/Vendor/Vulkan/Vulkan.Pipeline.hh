#pragma once

#include "PCH.hh"
#include "Vendor/Vulkan/Vulkan.RenderTarget.hh"
#include "Vendor/Vulkan/Vulkan.Shader.hh"

namespace ct::vulkan
{
	class Pipeline final
	{
	public:
		Pipeline(const Shader& vertex, const Shader& fragment);
		~Pipeline();
		Pipeline(Pipeline&& other) noexcept;
		Pipeline& operator=(Pipeline&& other) noexcept;

		inline vk::Pipeline handle() const
		{
			return PipelineHandle;
		}

		inline vk::PipelineLayout layout() const
		{
			return Layout;
		}

	private:
		vk::Pipeline PipelineHandle;
		vk::PipelineLayout Layout;

		vk::PipelineLayout createLayout();
		vk::PipelineShaderStageCreateInfo fillShaderStageInfo(vk::ShaderStageFlagBits stage, const Shader& shader);
		vk::PipelineRasterizationStateCreateInfo fillRasterizerInfo();
		vk::PipelineColorBlendAttachmentState fillColorBlendAttachment();
	};
}
