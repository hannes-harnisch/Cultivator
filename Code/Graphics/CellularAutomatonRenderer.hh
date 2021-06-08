#pragma once

#include "App/Window.hh"
#include "Utils/Rectangle.hh"
#include "Vendor/Vulkan/Vulkan.CommandList.hh"
#include "Vendor/Vulkan/Vulkan.Pipeline.hh"
#include "Vendor/Vulkan/Vulkan.PipelineLayout.hh"
#include "Vendor/Vulkan/Vulkan.SwapChain.hh"
#include "Vendor/Vulkan/Vulkan.Texture.hh"
#include "Vendor/Vulkan/Vulkan.Unique.hh"

namespace ct
{
	class CellularAutomatonRenderer
	{
	public:
		CellularAutomatonRenderer(Rectangle size, Window const& window);

		void draw();

	private:
		constexpr static uint32_t MaxFrames = 2;

		Rectangle universeSize;
		Rectangle windowViewport;
		Texture front, back;
		DeviceOwn<vk::Sampler, &vk::Device::destroySampler> sampler;
		DeviceOwn<vk::DescriptorSetLayout, &vk::Device::destroyDescriptorSetLayout> descSetLayout;
		PipelineLayout pipelineLayout;
		RenderPass universeUpdatePass;
		RenderPass presentPass;
		SwapChain swapChain;
		FrameBuffer frontBuffer;
		FrameBuffer backBuffer;
		Pipeline gameOfLife;
		Pipeline presentation;
		DeviceOwn<vk::DescriptorPool, &vk::Device::destroyDescriptorPool> descPool;
		vk::DescriptorSet descSet;
		std::vector<vk::Fence> imgInFlightFences;
		DeviceOwn<vk::Fence, &vk::Device::destroyFence> frameFences[MaxFrames];
		DeviceOwn<vk::Semaphore, &vk::Device::destroySemaphore> imgDoneSemaphores[MaxFrames];
		DeviceOwn<vk::Semaphore, &vk::Device::destroySemaphore> imgGetSemaphores[MaxFrames];
		uint32_t currentFrame {};
		CommandList commandLists[MaxFrames];

		CellularAutomatonRenderer(Rectangle size, Window const& window, Shader const& vertex);

		vk::Sampler makeSampler();
		void makeSyncObjects();
		void recordCommands(uint32_t imageIndex);
		vk::DescriptorSetLayout makeDescriptorSetLayout();
		vk::DescriptorPool makeDescriptorPool();
		vk::DescriptorSet makeDescriptorSet();
		void updateDescriptorSets(Texture const& tex);
	};
}
