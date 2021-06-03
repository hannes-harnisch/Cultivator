#include "PCH.hh"

#include "CellularAutomatonRenderer.hh"
#include "Vendor/Vulkan/Vulkan.GPUContext.hh"

namespace ct
{
	using namespace vulkan;

	CellularAutomatonRenderer::CellularAutomatonRenderer(Rectangle size) :
		CellularAutomatonRenderer(size, vulkan::Shader("ScreenQuad.vert.spv"))
	{}

	void CellularAutomatonRenderer::draw()
	{}

	vk::DescriptorSetLayout CellularAutomatonRenderer::makeDescriptorSetLayout()
	{
		auto dsl = vk::DescriptorSetLayoutBinding()
					   .setBinding(0)
					   .setDescriptorCount(1)
					   .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
					   .setStageFlags(vk::ShaderStageFlagBits::eFragment);

		std::array bindings {dsl, dsl.setBinding(1)};

		auto dslInfo	   = vk::DescriptorSetLayoutCreateInfo().setBindings(bindings);
		auto [res, handle] = GPUContext::device().createDescriptorSetLayout(dslInfo, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create descriptor set layout.");
		return handle;
	}

	CellularAutomatonRenderer::CellularAutomatonRenderer(Rectangle size, vulkan::Shader const& vertex) :
		descSetLayout(makeDescriptorSetLayout()),
		pipelineLayout(std::vector {descSetLayout}),
		gameOfLife(vertex, vulkan::Shader("GameOfLife.frag.spv"), pipelineLayout),
		presentation(vertex, vulkan::Shader("Presentation.frag.spv"), pipelineLayout),
		front(size),
		back(size)
	{}
}
