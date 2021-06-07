#pragma once

#include "PCH.hh"

#include "Vulkan.Unique.hh"

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
		DeviceOwn<vk::ShaderModule, &vk::Device::destroyShaderModule> shader;
	};
}
