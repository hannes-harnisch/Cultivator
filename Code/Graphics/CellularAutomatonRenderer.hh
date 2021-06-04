#pragma once

#include "App/Window.hh"
#include "Utils/Rectangle.hh"
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
		SwapChain swapChain;
		DeviceUnique<vk::DescriptorSetLayout, &vk::Device::destroyDescriptorSetLayout> descSetLayout;

		PipelineLayout pipelineLayout;
		Pipeline gameOfLife;
		Pipeline presentation;
		Texture front, back;

		vk::DescriptorSetLayout makeDescriptorSetLayout();

		CellularAutomatonRenderer(Rectangle size, Window const& window, Shader const& vertex);
	};
}
