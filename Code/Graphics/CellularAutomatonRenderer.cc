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
		universeUpdatePass(vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eShaderReadOnlyOptimal),
		presentPass(vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR),
		swapChain(window.handle(), windowViewport, presentPass),
		frontBuffer(size, universeUpdatePass, front.imageView()),
		backBuffer(size, universeUpdatePass, back.imageView()),
		gameOfLife(vertex, Shader("GameOfLife.frag.spv"), pipelineLayout, universeUpdatePass),
		presentation(vertex, Shader("Presentation.frag.spv"), pipelineLayout, universeUpdatePass),
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
		auto samplerInfo = vk::SamplerCreateInfo()
							   .setMagFilter(vk::Filter::eNearest)
							   .setMinFilter(vk::Filter::eNearest)
							   .setAddressModeU(vk::SamplerAddressMode::eClampToBorder)
							   .setAddressModeV(vk::SamplerAddressMode::eClampToBorder)
							   .setAddressModeW(vk::SamplerAddressMode::eClampToBorder)
							   .setAnisotropyEnable(false)
							   .setMaxAnisotropy(1.0f)
							   .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
							   .setUnnormalizedCoordinates(false)
							   .setCompareEnable(false)
							   .setCompareOp(vk::CompareOp::eAlways);
		auto [res, handle] = GPUContext::device().createSampler(samplerInfo, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create sampler.");
		return handle;
	}

	vk::DescriptorSetLayout CellularAutomatonRenderer::makeDescriptorSetLayout()
	{
		auto dsl = vk::DescriptorSetLayoutBinding()
					   .setBinding(0)
					   .setDescriptorCount(1)
					   .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
					   .setStageFlags(vk::ShaderStageFlagBits::eFragment);

		std::array bindings {dsl};

		auto dslInfo	   = vk::DescriptorSetLayoutCreateInfo().setBindings(bindings);
		auto [res, handle] = GPUContext::device().createDescriptorSetLayout(dslInfo, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create descriptor set layout.");
		return handle;
	}

	vk::DescriptorPool CellularAutomatonRenderer::makeDescriptorPool()
	{
		std::array poolSizes {
			vk::DescriptorPoolSize().setType(vk::DescriptorType::eCombinedImageSampler).setDescriptorCount(1)};

		auto poolInfo	 = vk::DescriptorPoolCreateInfo().setPoolSizes(poolSizes).setMaxSets(2);
		auto [res, pool] = GPUContext::device().createDescriptorPool(poolInfo, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create descriptor pool.");
		return pool;
	}

	vk::DescriptorSet CellularAutomatonRenderer::makeDescriptorSetForSampler(Texture const& tex)
	{
		std::array setLayouts {descSetLayout.get()};
		auto allocInfo	 = vk::DescriptorSetAllocateInfo().setDescriptorPool(descPool).setSetLayouts(setLayouts);
		auto [res, sets] = GPUContext::device().allocateDescriptorSets(allocInfo, Loader::get());
		ctAssertResult(res, "Failed to allocate descriptor sets.");

		auto descImgInfo = vk::DescriptorImageInfo()
							   .setSampler(sampler)
							   .setImageView(tex.imageView())
							   .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
		std::array descWrites {vk::WriteDescriptorSet()
								   .setDstSet(sets[0])
								   .setDstBinding(0)
								   .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
								   .setDescriptorCount(1)
								   .setImageInfo(descImgInfo)};
		GPUContext::device().updateDescriptorSets(descWrites, {}, Loader::get());

		return sets[0];
	}

	void CellularAutomatonRenderer::makeSyncObjects()
	{
		auto fenceInfo = vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled);
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
		size_t size = 4 * back.size().area();
		Buffer stage(size, vk::BufferUsageFlagBits::eTransferSrc,
					 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
		void* stageTarget;
		ctAssertResult(GPUContext::device().mapMemory(stage.memory(), 0, size, {}, &stageTarget, Loader::get()),
					   "Failed to map memory.");
		unsigned* pixels = static_cast<unsigned*>(stageTarget);
		for(size_t i = 0; i < size / 4; ++i)
			*pixels++ = std::rand() % 2 == 0 ? 0xFFFFFFFF : 0;

		GPUContext::device().unmapMemory(stage.memory(), Loader::get());

		CommandList list;
		list.begin();
		list.pushImageBarrier(front, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal);
		list.pushImageBarrier(back, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
		list.end();
		GPUContext::graphicsQueue().submitSync(list.handle());

		list.begin();
		list.copyBufferToTexture(stage, back);
		list.end();
		GPUContext::graphicsQueue().submitSync(list.handle());

		list.begin();
		list.pushImageBarrier(back, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
		list.end();
		GPUContext::graphicsQueue().submitSync(list.handle());
	}

	void CellularAutomatonRenderer::draw()
	{
		std::array frameFence {frameFences[currentFrame].get()};
		ctAssertResult(GPUContext::device().waitForFences(frameFence, true, UINT64_MAX, Loader::get()),
					   "Failed to wait for fences.");

		uint32_t imgIndex = swapChain.getNextImageIndex(imgGetSemaphores[currentFrame]);
		recordCommands(imgIndex);

		if(imgInFlightFences[imgIndex])
		{
			std::array imgFence {imgInFlightFences[imgIndex]};
			ctAssertResult(GPUContext::device().waitForFences(imgFence, true, UINT64_MAX, Loader::get()),
						   "Failed to wait for fences.");
		}
		imgInFlightFences[imgIndex] = frameFences[currentFrame];
		GPUContext::device().resetFences(frameFence, Loader::get());

		GPUContext::graphicsQueue().submit(commandLists[imgIndex].handle(), imgGetSemaphores[currentFrame],
										   imgDoneSemaphores[currentFrame], frameFences[currentFrame]);

		swapChain.present(imgIndex, imgDoneSemaphores[currentFrame]);
		currentFrame = (currentFrame + 1) % MaxFrames;
	}

	void CellularAutomatonRenderer::recordCommands(uint32_t imgIndex)
	{
		auto& com = commandLists[imgIndex];

		com.begin();
		com.pushImageBarrier(currentFrame ? back : front, vk::ImageLayout::eShaderReadOnlyOptimal,
							 vk::ImageLayout::eColorAttachmentOptimal);

		com.beginRenderPass(back.size(), universeUpdatePass, currentFrame ? backBuffer : frontBuffer);
		com.bindViewport(back.size());
		com.bindScissor(back.size());
		com.bindDescriptorSets(pipelineLayout, {currentFrame ? frontDescSet : backDescSet});
		com.bindPipeline(gameOfLife);
		com.draw();
		com.endRenderPass();

		com.pushImageBarrier(currentFrame ? back : front, vk::ImageLayout::eColorAttachmentOptimal,
							 vk::ImageLayout::eShaderReadOnlyOptimal);

		com.beginRenderPass(windowViewport, presentPass, swapChain.getFrameBuffer(imgIndex));
		com.bindViewport(windowViewport);
		com.bindScissor(windowViewport);
		com.bindDescriptorSets(pipelineLayout, {currentFrame ? backDescSet : frontDescSet});
		com.bindPipeline(presentation);
		com.draw();
		com.endRenderPass();

		com.end();
	}
}
