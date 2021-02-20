#pragma once

#include "PCH.hh"
#include "Vendor/Vulkan/Vulkan.Shader.hh"

namespace ct::vulkan
{
	class Pipeline final
	{
		Pipeline(const Shader& vertex, const Shader& fragment);
		~Pipeline();
		Pipeline(Pipeline&& other) noexcept;
		Pipeline& operator=(Pipeline&& other) noexcept;

		inline vk::Pipeline handle() const
		{
			return PipelineHandle;
		}

	private:
		vk::Pipeline PipelineHandle;
		vk::PipelineLayout Layout;

		std::array<vk::PipelineShaderStageCreateInfo, 2> fillShaderStages(const Shader& vertex, const Shader& fragment);
		vk::PipelineRasterizationStateCreateInfo fillRasterizerInfo();
	};
}
