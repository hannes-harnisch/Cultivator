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

		SwapChain swapChain;
		DeviceUnique<vk::DescriptorSetLayout, &vk::Device::destroyDescriptorSetLayout> descSetLayout;
		PipelineLayout pipelineLayout;
		Pipeline gameOfLife;
		Pipeline presentation;
		Texture front, back;
		std::vector<vk::Fence> imgInFlightFences;
		DeviceUnique<vk::Fence, &vk::Device::destroyFence> frameFences[MaxFrames];
		DeviceUnique<vk::Semaphore, &vk::Device::destroySemaphore> imgDoneSemaphores[MaxFrames];
		DeviceUnique<vk::Semaphore, &vk::Device::destroySemaphore> imgGetSemaphores[MaxFrames];
		uint32_t currentFrame {};
		CommandList commandList;

		CellularAutomatonRenderer(Rectangle size, Window const& window, Shader const& vertex);

		void makeSyncObjects();
		void recordCommands();
		vk::DescriptorSetLayout makeDescriptorSetLayout();
	};
}
