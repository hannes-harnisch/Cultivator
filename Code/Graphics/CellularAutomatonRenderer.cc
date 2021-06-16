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
		ctAssertResult(GPUContext::device().waitIdle(Loader::get()), "Failed to wait for idle device.");
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
		gameOfLife(vertex, Shader("DayAndNight.frag.spv"), pipelineLayout, simulationPass),
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
		vk::SamplerCreateInfo info;
		info.addressModeU			 = vk::SamplerAddressMode::eClampToBorder;
		info.addressModeV			 = vk::SamplerAddressMode::eClampToBorder;
		info.addressModeW			 = vk::SamplerAddressMode::eClampToBorder;
		info.anisotropyEnable		 = true;
		info.maxAnisotropy			 = 16.0f;
		info.borderColor			 = vk::BorderColor::eIntOpaqueBlack;
		info.unnormalizedCoordinates = false;
		info.compareEnable			 = false;
		info.compareOp				 = vk::CompareOp::eAlways;

		auto [res, handle] = GPUContext::device().createSampler(info, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create sampler.");
		return handle;
	}

	vk::DescriptorSetLayout CellularAutomatonRenderer::makeDescriptorSetLayout()
	{
		vk::DescriptorSetLayoutBinding binding;
		binding.binding			= 0;
		binding.descriptorCount = 1;
		binding.descriptorType	= vk::DescriptorType::eCombinedImageSampler;
		binding.stageFlags		= vk::ShaderStageFlagBits::eFragment;

		vk::DescriptorSetLayoutCreateInfo info;
		info.bindingCount = 1;
		info.pBindings	  = &binding;

		auto [res, handle] = GPUContext::device().createDescriptorSetLayout(info, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create descriptor set layout.");
		return handle;
	}

	vk::DescriptorPool CellularAutomatonRenderer::makeDescriptorPool()
	{
		vk::DescriptorPoolSize poolSize;
		poolSize.type			 = vk::DescriptorType::eCombinedImageSampler;
		poolSize.descriptorCount = 1;

		vk::DescriptorPoolCreateInfo info;
		info.maxSets	   = 2;
		info.poolSizeCount = 1;
		info.pPoolSizes	   = &poolSize;

		auto [res, pool] = GPUContext::device().createDescriptorPool(info, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create descriptor pool.");
		return pool;
	}

	vk::DescriptorSet CellularAutomatonRenderer::makeDescriptorSetForSampler(Texture const& tex)
	{
		auto layout = descSetLayout.get();
		vk::DescriptorSetAllocateInfo alloc;
		alloc.descriptorPool	 = descPool;
		alloc.descriptorSetCount = 1;
		alloc.pSetLayouts		 = &layout;
		auto [res, sets]		 = GPUContext::device().allocateDescriptorSets(alloc, Loader::get());
		ctAssertResult(res, "Failed to allocate descriptor sets.");

		vk::DescriptorImageInfo info;
		info.sampler	 = sampler;
		info.imageView	 = tex.imageView();
		info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

		vk::WriteDescriptorSet descWrite;
		descWrite.dstSet		  = sets[0];
		descWrite.dstBinding	  = 0;
		descWrite.descriptorCount = 1;
		descWrite.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
		descWrite.pImageInfo	  = &info;
		GPUContext::device().updateDescriptorSets(descWrite, {}, Loader::get());

		return sets[0];
	}

	void CellularAutomatonRenderer::makeSyncObjects()
	{
		vk::FenceCreateInfo fenceInfo;
		fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

		vk::SemaphoreCreateInfo semaphoreInfo;
		for(auto& sem : imgGetSemaphores)
		{
			auto [res, semaphore] = GPUContext::device().createSemaphore(semaphoreInfo, nullptr, Loader::get());
			ctEnsureResult(res, "Failed to create image-get semaphore.");
			sem = semaphore;
		}
		for(auto& sem : imgDoneSemaphores)
		{
			auto [res, semaphore] = GPUContext::device().createSemaphore(semaphoreInfo, nullptr, Loader::get());
			ctEnsureResult(res, "Failed to create image-done semaphore.");
			sem = semaphore;
		}
		for(auto& fence : frameFences)
		{
			auto [res, handle] = GPUContext::device().createFence(fenceInfo, nullptr, Loader::get());
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
		ctAssertResult(GPUContext::device().mapMemory(stage.memory(), 0, size, {}, &stageTarget, Loader::get()),
					   "Failed to map memory.");

		unsigned* pixels = static_cast<unsigned*>(stageTarget);
		for(size_t i = 0; i < size / 4; ++i)
		{
			*pixels++ = std::rand() % 2 == 0 ? 0xFFFFFFFF : 0;
		}
		GPUContext::device().unmapMemory(stage.memory(), Loader::get());

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

		ctAssertResult(GPUContext::device().waitForFences(frameFences[curFrame].get(), true, UINT64_MAX, Loader::get()),
					   "Failed to wait for fences.");

		uint32_t imgIndex = swapChain.getNextImageIndex(imgGetSemaphores[curFrame]);
		recordCommands(imgIndex);

		if(imgInFlightFences[imgIndex])
			ctAssertResult(GPUContext::device().waitForFences(imgInFlightFences[imgIndex], true, UINT64_MAX, Loader::get()),
						   "Failed to wait for fences.");

		imgInFlightFences[imgIndex] = frameFences[curFrame];
		GPUContext::device().resetFences(frameFences[curFrame].get(), Loader::get());

		GPUContext::graphicsQueue().submit(commandLists[imgIndex].handle(), imgGetSemaphores[curFrame],
										   imgDoneSemaphores[curFrame], frameFences[curFrame]);

		swapChain.present(imgIndex, imgDoneSemaphores[curFrame]);
		curFrame = (curFrame + 1) % MaxFrames;
	}

	void CellularAutomatonRenderer::recordCommands(uint32_t imgIndex)
	{
		auto& com = commandLists[imgIndex];

		com.reset();
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
