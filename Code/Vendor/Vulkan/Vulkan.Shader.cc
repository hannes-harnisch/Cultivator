#include "PCH.hh"

#include "Utils/File.hh"
#include "Vulkan.GPUContext.hh"
#include "Vulkan.Shader.hh"

namespace ct
{
	Shader::Shader(std::string_view const filePath)
	{
		auto bytecode = File::loadBinary(filePath);

		vk::ShaderModuleCreateInfo shaderInfo;
		shaderInfo.codeSize = bytecode.size();
		shaderInfo.pCode	= reinterpret_cast<uint32_t*>(bytecode.data());

		auto [res, handle] = GPUContext::device().createShaderModule(shaderInfo, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create Vulkan shader module.");
		shader = handle;
	}
}
