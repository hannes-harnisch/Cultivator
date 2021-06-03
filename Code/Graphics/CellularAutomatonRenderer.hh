#pragma once

#include "Utils/Rectangle.hh"
#include "Vendor/Vulkan/Vulkan.Pipeline.hh"
#include "Vendor/Vulkan/Vulkan.PipelineLayout.hh"
#include "Vendor/Vulkan/Vulkan.Texture.hh"

namespace ct
{
	class CellularAutomatonRenderer
	{
	public:
		CellularAutomatonRenderer(Rectangle size);

		void draw();

	private:
		vk::DescriptorSetLayout descSetLayout;

		vulkan::PipelineLayout pipelineLayout;
		vulkan::Pipeline gameOfLife;
		vulkan::Pipeline presentation;
		vulkan::Texture front, back;

		vk::DescriptorSetLayout makeDescriptorSetLayout();

		CellularAutomatonRenderer(Rectangle size, vulkan::Shader const& vertex);
	};
}
