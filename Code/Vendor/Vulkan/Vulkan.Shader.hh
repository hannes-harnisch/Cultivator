#pragma once

#include "PCH.hh"

#include "Vendor/Vulkan/Vulkan.Unique.hh"

namespace ct::vulkan
{
	class Shader final
	{
	public:
		static constexpr auto EntryPoint = "main";

		Shader(std::string_view filePath);

		vk::ShaderModule handle() const
		{
			return ShaderModule;
		}

	private:
		DeviceUnique<vk::ShaderModule, &vk::Device::destroyShaderModule> ShaderModule;

		static vk::ShaderModule createShader(std::string_view filePath);
	};
}
