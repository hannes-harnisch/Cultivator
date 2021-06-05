#include "PCH.hh"

#include "Utils/File.hh"
#include "Vendor/Vulkan/Vulkan.GPUContext.hh"
#include "Vulkan.Shader.hh"

namespace ct
{
	Shader::Shader(std::string_view const filePath)
	{
		auto bytecode = File::loadBinary(filePath);
		auto shaderInfo =
			vk::ShaderModuleCreateInfo().setCodeSize(bytecode.size()).setPCode(reinterpret_cast<uint32_t*>(bytecode.data()));
		auto [res, sh] = GPUContext::device().createShaderModule(shaderInfo, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create Vulkan shader module.");
		shader = sh;
	}
}
