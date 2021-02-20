#pragma once

#include "PCH.hh"

namespace ct::vulkan
{
	class Shader final
	{
	public:
		Shader(const std::string& filePath);
		Shader(Shader&& other) noexcept;
		~Shader();
		Shader& operator=(Shader&& other) noexcept;

		inline vk::ShaderModule handle() const
		{
			return ShaderModule;
		}

	private:
		vk::ShaderModule ShaderModule;
	};
}
