#include "PCH.hh"
#include "Vulkan.Shader.hh"

#include "Utils/File.hh"
#include "Vendor/Vulkan/Vulkan.GraphicsContext.hh"

namespace ct::vulkan
{
	Shader::Shader(const std::string& filePath)
	{
		auto bytecode {File::loadBinary(filePath)};
		auto shaderInfo {vk::ShaderModuleCreateInfo()
							 .setCodeSize(bytecode.size())
							 .setPCode(reinterpret_cast<uint32_t*>(bytecode.data()))};

		auto [result, shader] {GraphicsContext::device().createShaderModule(shaderInfo)};
		ctAssertResult(result, "Failed to create Vulkan shader module.");
		ShaderModule = shader;
	}

	Shader::Shader(Shader&& other) noexcept : ShaderModule {std::exchange(other.ShaderModule, nullptr)}
	{}

	Shader::~Shader()
	{
		GraphicsContext::device().destroyShaderModule(ShaderModule);
	}

	Shader& Shader::operator=(Shader&& other) noexcept
	{
		std::swap(ShaderModule, other.ShaderModule);
		return *this;
	}
}
