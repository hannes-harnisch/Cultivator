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
		std::array fence {frameFences[currentFrame].get()};
		GPUContext::device().waitForFences(fence, true, std::numeric_limits<uint64_t>::max(), Loader::get());

		uint32_t imgIndex = swapChain.getNextImageIndex(imgGetSemaphores[currentFrame]);

		if(imgInFlightFences[imgIndex])
		{
			std::array fence {imgInFlightFences[imgIndex]};
			ctAssertResult(GPUContext::device().waitForFences(fence, true, std::numeric_limits<uint64_t>::max(), Loader::get()),
						   "Failed to wait for fences.");
		}
		imgInFlightFences[imgIndex] = frameFences[currentFrame];
		GPUContext::device().resetFences(fence, Loader::get());

		GPUContext::graphicsQueue().submit(commandList.handle(), imgGetSemaphores[currentFrame],
										   imgDoneSemaphores[currentFrame], frameFences[currentFrame]);

		swapChain.present(imgIndex, imgDoneSemaphores[currentFrame]);
		currentFrame = (currentFrame + 1) % MaxFrames;
	}

	CellularAutomatonRenderer::CellularAutomatonRenderer(Rectangle size, Window const& window, Shader const& vertex) :
		swapChain(window.handle(), window.getViewport()),
		descSetLayout(makeDescriptorSetLayout()),
		pipelineLayout(std::vector {descSetLayout.get()}),
		gameOfLife(vertex, Shader("GameOfLife.frag.spv"), pipelineLayout),
		presentation(vertex, Shader("Presentation.frag.spv"), pipelineLayout),
		front(size),
		back(size),
		commandList()
	{
		makeSyncObjects();
	}

	void CellularAutomatonRenderer::makeSyncObjects()
	{
		auto fenceInfo = vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled);
		vk::SemaphoreCreateInfo semaphoreInfo;
		for(int i {}; i < MaxFrames; ++i)
		{
			auto [getSemRes, imageGetSemaphore] = GPUContext::device().createSemaphore(semaphoreInfo, nullptr, Loader::get());
			ctEnsureResult(getSemRes, "Failed to create image-get semaphore.");
			imgGetSemaphores[i] = imageGetSemaphore;

			auto [doneSemRes, imageDoneSemaphore] = GPUContext::device().createSemaphore(semaphoreInfo, nullptr, Loader::get());
			ctEnsureResult(doneSemRes, "Failed to create image-done semaphore.");
			imgDoneSemaphores[i] = imageDoneSemaphore;

			auto [fenceRes, fence] = GPUContext::device().createFence(fenceInfo, nullptr, Loader::get());
			ctEnsureResult(fenceRes, "Failed to create fence.");
			frameFences[i] = fence;
		}
		imgInFlightFences.resize(swapChain.getImageCount());
	}

	void CellularAutomatonRenderer::recordCommands()
	{
		commandList.begin();
		commandList.end();
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
}
