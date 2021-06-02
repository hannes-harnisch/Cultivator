#include "PCH.hh"

#include "Utils/File.hh"
#include "Vendor/Vulkan/Vulkan.GPUContext.hh"
#include "Vulkan.Shader.hh"

namespace ct::vulkan
{
	Shader::Shader(std::string_view filePath) : ShaderModule(createShader(filePath))
	{}

	vk::ShaderModule Shader::createShader(std::string_view filePath)
	{
		auto bytecode = File::loadBinary(filePath);
		auto shaderInfo =
			vk::ShaderModuleCreateInfo().setCodeSize(bytecode.size()).setPCode(reinterpret_cast<uint32_t*>(bytecode.data()));
		auto [res, shader] = GPUContext::device().createShaderModule(shaderInfo, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create Vulkan shader module.");
		return shader;
	}
}
