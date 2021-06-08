#include "PCH.hh"

#include "CellularAutomatonRenderer.hh"
#include "Vendor/Vulkan/Vulkan.GPUContext.hh"

namespace ct
{
	CellularAutomatonRenderer::CellularAutomatonRenderer(Rectangle const size, Window const& window) :
		CellularAutomatonRenderer(size, window, Shader("ScreenQuad.vert.spv"))
	{}

	void CellularAutomatonRenderer::draw()
	{
		updateDescriptorSets(currentFrame ? front : back);

		std::array fence {frameFences[currentFrame].get()};
		ctAssertResult(GPUContext::device().waitForFences(fence, true, std::numeric_limits<uint64_t>::max(), Loader::get()),
					   "Failed to wait for fences.");

		uint32_t imgIndex = swapChain.getNextImageIndex(imgGetSemaphores[currentFrame]);
		recordCommands(imgIndex);

		if(imgInFlightFences[imgIndex])
		{
			std::array fence {imgInFlightFences[imgIndex]};
			ctAssertResult(GPUContext::device().waitForFences(fence, true, std::numeric_limits<uint64_t>::max(), Loader::get()),
						   "Failed to wait for fences.");
		}
		imgInFlightFences[imgIndex] = frameFences[currentFrame];
		GPUContext::device().resetFences(fence, Loader::get());

		GPUContext::graphicsQueue().submit(commandLists[imgIndex].handle(), imgGetSemaphores[currentFrame],
										   imgDoneSemaphores[currentFrame], frameFences[currentFrame]);

		swapChain.present(imgIndex, imgDoneSemaphores[currentFrame]);
		currentFrame = (currentFrame + 1) % MaxFrames;
	}

	CellularAutomatonRenderer::CellularAutomatonRenderer(Rectangle const size, Window const& window, Shader const& vertex) :
		universeSize(size),
		windowViewport(window.getViewport()),
		front(size),
		back(size),
		sampler(makeSampler()),
		descSetLayout(makeDescriptorSetLayout()),
		pipelineLayout(std::vector {descSetLayout.get()}),
		swapChain(window.handle(), windowViewport, presentPass),
		frontBuffer(size, universeUpdatePass, front.imageView()),
		backBuffer(size, universeUpdatePass, back.imageView()),
		gameOfLife(vertex, Shader("GameOfLife.frag.spv"), pipelineLayout, universeUpdatePass),
		presentation(vertex, Shader("Presentation.frag.spv"), pipelineLayout, universeUpdatePass),
		descPool(makeDescriptorPool()),
		descSet(makeDescriptorSet())
	{
		makeSyncObjects();
	}

	vk::Sampler CellularAutomatonRenderer::makeSampler()
	{
		auto samplerInfo = vk::SamplerCreateInfo()
							   .setMagFilter(vk::Filter::eLinear)
							   .setMinFilter(vk::Filter::eLinear)
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

	void CellularAutomatonRenderer::recordCommands(uint32_t imageIndex)
	{
		std::array frameBuf {&frontBuffer, &backBuffer};
		int i = 0;
		for(auto& com : commandLists)
		{
			com.reset();
			com.begin();

			com.beginRenderPass(universeUpdatePass, *frameBuf[i]);
			com.bindViewport(universeSize);
			com.bindScissor(universeSize);
			com.bindDescriptorSets(pipelineLayout, {descSet});
			com.bindPipeline(gameOfLife);
			com.draw();
			com.endRenderPass();

			com.beginRenderPass(presentPass, swapChain.getFrameBuffer(imageIndex));
			com.bindViewport(windowViewport);
			com.bindScissor(windowViewport);
			com.bindPipeline(presentation);
			com.draw();
			com.endRenderPass();

			com.end();
			++i;
		}
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

		auto poolInfo	 = vk::DescriptorPoolCreateInfo().setPoolSizes(poolSizes).setMaxSets(1);
		auto [res, pool] = GPUContext::device().createDescriptorPool(poolInfo, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create descriptor pool.");
		return pool;
	}

	vk::DescriptorSet CellularAutomatonRenderer::makeDescriptorSet()
	{
		std::array setLayouts {descSetLayout.get()};
		auto allocInfo	 = vk::DescriptorSetAllocateInfo().setDescriptorPool(descPool).setSetLayouts(setLayouts);
		auto [res, sets] = GPUContext::device().allocateDescriptorSets(allocInfo, Loader::get());
		ctAssertResult(res, "Failed to allocate descriptor sets.");
		return sets[0];
	}

	void CellularAutomatonRenderer::updateDescriptorSets(Texture const& tex)
	{
		auto descImgInfo = vk::DescriptorImageInfo()
							   .setSampler(sampler)
							   .setImageView(tex.imageView())
							   .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
		std::array descWrites {vk::WriteDescriptorSet()
								   .setDstSet(descSet)
								   .setDstBinding(0)
								   .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
								   .setDescriptorCount(1)
								   .setImageInfo(descImgInfo)};
		GPUContext::device().updateDescriptorSets(descWrites, {}, Loader::get());
	}
}
