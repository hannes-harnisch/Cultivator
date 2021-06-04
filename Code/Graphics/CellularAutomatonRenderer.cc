#include "PCH.hh"

#include "CellularAutomatonRenderer.hh"
#include "Vendor/Vulkan/Vulkan.GPUContext.hh"

namespace ct
{
	CellularAutomatonRenderer::CellularAutomatonRenderer(Rectangle size, Window const& window) :
		CellularAutomatonRenderer(size, window, Shader("ScreenQuad.vert.spv"))
	{}

	void CellularAutomatonRenderer::draw()
	{
		// queue.submit(commandList);
		swapChain.present();
	}

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

	CellularAutomatonRenderer::CellularAutomatonRenderer(Rectangle size, Window const& window, Shader const& vertex) :
		swapChain(window.handle(), window.getViewport()),
		descSetLayout(makeDescriptorSetLayout()),
		pipelineLayout(std::vector {descSetLayout.get()}),
		gameOfLife(vertex, Shader("GameOfLife.frag.spv"), pipelineLayout),
		presentation(vertex, Shader("Presentation.frag.spv"), pipelineLayout),
		front(size),
		back(size)
	{}
}
