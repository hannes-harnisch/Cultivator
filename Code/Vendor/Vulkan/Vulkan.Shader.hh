#pragma once

#include "PCH.hh"

#include "Vendor/Vulkan/Vulkan.Unique.hh"

namespace ct
{
	class Shader final
	{
	public:
		static constexpr auto EntryPoint = "main";

		Shader(std::string_view filePath);

		vk::ShaderModule handle() const
		{
			return shader;
		}

	private:
		DeviceUnique<vk::ShaderModule, &vk::Device::destroyShaderModule> shader;

		static vk::ShaderModule makeShader(std::string_view filePath);
	};
}
