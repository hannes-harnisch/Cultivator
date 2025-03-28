#include "PCH.hh"

#include "CellularAutomatonRenderer.hh"
#include "Vendor/Vulkan/Vulkan.Buffer.hh"
#include "Vendor/Vulkan/Vulkan.GPUContext.hh"

namespace ct
{
	CellularAutomatonRenderer::CellularAutomatonRenderer(Rectangle const size, Window const& window) :
		CellularAutomatonRenderer(size, window, Shader("ScreenQuad.vert.spv"))
	{}

	CellularAutomatonRenderer::~CellularAutomatonRenderer()
	{
		ctAssertResult(GPUContext::device().waitIdle(), "Failed to wait for idle device.");
	}

	CellularAutomatonRenderer::CellularAutomatonRenderer(Rectangle const size, Window const& window, Shader const& vertex) :
		windowViewport(window.getViewport()),
		front(size),
		back(size),
		sampler(makeSampler()),
		descSetLayout(makeDescriptorSetLayout()),
		pipelineLayout(std::vector {descSetLayout.get()}),
		simulationPass(vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eShaderReadOnlyOptimal),
		presentPass(vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR),
		swapChain(window.handle(), windowViewport, presentPass),
		frontBuffer(size, simulationPass, front.imageView()),
		backBuffer(size, simulationPass, back.imageView()),
		gameOfLife(vertex, Shader("GameOfLife.frag.spv"), pipelineLayout, simulationPass),
		presentation(vertex, Shader("Presentation.frag.spv"), pipelineLayout, simulationPass),
		descPool(makeDescriptorPool()),
		frontDescSet(makeDescriptorSetForSampler(front)),
		backDescSet(makeDescriptorSetForSampler(back)),
		commandLists(swapChain.getImageCount())
	{
		makeSyncObjects();
		prepareTextures();
	}

	vk::Sampler CellularAutomatonRenderer::makeSampler()
	{
		vk::SamplerCreateInfo info {
			.addressModeU			 = vk::SamplerAddressMode::eClampToBorder,
			.addressModeV			 = vk::SamplerAddressMode::eClampToBorder,
			.addressModeW			 = vk::SamplerAddressMode::eClampToBorder,
			.anisotropyEnable		 = true,
			.maxAnisotropy			 = 16.0f,
			.compareEnable			 = false,
			.compareOp				 = vk::CompareOp::eAlways,
			.borderColor			 = vk::BorderColor::eIntOpaqueBlack,
			.unnormalizedCoordinates = false,
		};
		auto [res, handle] = GPUContext::device().createSampler(info);
		ctAssertResult(res, "Failed to create sampler.");
		return handle;
	}

	vk::DescriptorSetLayout CellularAutomatonRenderer::makeDescriptorSetLayout()
	{
		vk::DescriptorSetLayoutBinding binding {
			.binding		 = 0,
			.descriptorType	 = vk::DescriptorType::eCombinedImageSampler,
			.descriptorCount = 1,
			.stageFlags		 = vk::ShaderStageFlagBits::eFragment,
		};
		vk::DescriptorSetLayoutCreateInfo info {
			.bindingCount = 1,
			.pBindings	  = &binding,
		};
		auto [res, handle] = GPUContext::device().createDescriptorSetLayout(info);
		ctAssertResult(res, "Failed to create descriptor set layout.");
		return handle;
	}

	vk::DescriptorPool CellularAutomatonRenderer::makeDescriptorPool()
	{
		vk::DescriptorPoolSize poolSize {
			.type			 = vk::DescriptorType::eCombinedImageSampler,
			.descriptorCount = 2,
		};
		vk::DescriptorPoolCreateInfo info {
			.maxSets	   = 2,
			.poolSizeCount = 1,
			.pPoolSizes	   = &poolSize,
		};
		auto [res, pool] = GPUContext::device().createDescriptorPool(info);
		ctAssertResult(res, "Failed to create descriptor pool.");
		return pool;
	}

	vk::DescriptorSet CellularAutomatonRenderer::makeDescriptorSetForSampler(Texture const& tex)
	{
		auto layout = descSetLayout.get();
		vk::DescriptorSetAllocateInfo alloc {
			.descriptorPool		= descPool,
			.descriptorSetCount = 1,
			.pSetLayouts		= &layout,
		};
		auto [res, sets] = GPUContext::device().allocateDescriptorSets(alloc);
		ctAssertResult(res, "Failed to allocate descriptor sets.");

		vk::DescriptorImageInfo info {
			.sampler	 = sampler,
			.imageView	 = tex.imageView(),
			.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
		};
		vk::WriteDescriptorSet descWrite {
			.dstSet			 = sets[0],
			.dstBinding		 = 0,
			.descriptorCount = 1,
			.descriptorType	 = vk::DescriptorType::eCombinedImageSampler,
			.pImageInfo		 = &info,
		};
		GPUContext::device().updateDescriptorSets(descWrite, {});

		return sets[0];
	}

	void CellularAutomatonRenderer::makeSyncObjects()
	{
		vk::FenceCreateInfo fenceInfo {
			.flags = vk::FenceCreateFlagBits::eSignaled,
		};
		vk::SemaphoreCreateInfo semaphoreInfo;
		for(auto& sem : imgGetSemaphores)
		{
			auto [res, semaphore] = GPUContext::device().createSemaphore(semaphoreInfo);
			ctEnsureResult(res, "Failed to create image-get semaphore.");
			sem = semaphore;
		}
		for(auto& sem : imgDoneSemaphores)
		{
			auto [res, semaphore] = GPUContext::device().createSemaphore(semaphoreInfo);
			ctEnsureResult(res, "Failed to create image-done semaphore.");
			sem = semaphore;
		}
		for(auto& fence : frameFences)
		{
			auto [res, handle] = GPUContext::device().createFence(fenceInfo);
			ctEnsureResult(res, "Failed to create fence.");
			fence = handle;
		}
		imgInFlightFences.resize(swapChain.getImageCount());
	}

	void CellularAutomatonRenderer::prepareTextures()
	{
		std::srand(static_cast<unsigned>(std::time(nullptr)));
		size_t size = static_cast<size_t>(4) * back.size().area();
		Buffer stage(size, vk::BufferUsageFlagBits::eTransferSrc,
					 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

		void* stageTarget;
		ctAssertResult(GPUContext::device().mapMemory(stage.memory(), 0, size, {}, &stageTarget), "Failed to map memory.");

		unsigned* pixels = static_cast<unsigned*>(stageTarget);
		for(size_t i = 0; i < size / 4; ++i)
		{
			*pixels++ = std::rand() % 2 == 0 ? 0xFFFFFFFF : 0;
		}
		GPUContext::device().unmapMemory(stage.memory());

		CommandList list;
		list.begin();
		list.transitionTexture(front, vk::ImageLayout::eColorAttachmentOptimal);
		list.transitionTexture(back, vk::ImageLayout::eTransferDstOptimal);
		list.end();
		GPUContext::graphicsQueue().submitSync(list.handle());

		list.begin();
		list.copyBufferToTexture(stage, back);
		list.end();
		GPUContext::graphicsQueue().submitSync(list.handle());

		list.begin();
		list.transitionTexture(back, vk::ImageLayout::eShaderReadOnlyOptimal);
		list.end();
		GPUContext::graphicsQueue().submitSync(list.handle());
	}

	void CellularAutomatonRenderer::draw()
	{
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1ms);

		ctAssertResult(GPUContext::device().waitForFences(frameFences[curFrame].get(), true, UINT64_MAX),
					   "Failed to wait for fences.");

		uint32_t imgIndex = swapChain.getNextImageIndex(imgGetSemaphores[curFrame]);
		recordCommands(imgIndex);

		if(imgInFlightFences[imgIndex])
			ctAssertResult(GPUContext::device().waitForFences(imgInFlightFences[imgIndex], true, UINT64_MAX),
						   "Failed to wait for fences.");

		imgInFlightFences[imgIndex] = frameFences[curFrame];
		GPUContext::device().resetFences(frameFences[curFrame].get());

		GPUContext::graphicsQueue().submit(commandLists[imgIndex].handle(), imgGetSemaphores[curFrame],
										   imgDoneSemaphores[curFrame], frameFences[curFrame]);

		swapChain.present(imgIndex, imgDoneSemaphores[curFrame]);
		curFrame = (curFrame + 1) % MaxFrames;
	}

	void CellularAutomatonRenderer::recordCommands(uint32_t imgIndex)
	{
		auto& com = commandLists[imgIndex];

		com.begin();
		com.transitionTexture(curFrame ? back : front, vk::ImageLayout::eColorAttachmentOptimal);

		com.beginRenderPass(back.size(), simulationPass, curFrame ? backBuffer : frontBuffer);
		com.bindViewport(back.size());
		com.bindScissor(back.size());
		com.bindDescriptorSets(pipelineLayout, {curFrame ? frontDescSet : backDescSet});
		com.bindPipeline(gameOfLife);
		com.draw();
		com.endRenderPass();

		com.transitionTexture(curFrame ? back : front, vk::ImageLayout::eShaderReadOnlyOptimal);

		com.beginRenderPass(windowViewport, presentPass, swapChain.getFrameBuffer(imgIndex));
		com.bindViewport(windowViewport);
		com.bindScissor(windowViewport);
		com.bindDescriptorSets(pipelineLayout, {curFrame ? backDescSet : frontDescSet});
		com.bindPipeline(presentation);
		com.draw();
		com.endRenderPass();

		com.end();
	}
}
