#pragma once

#include "Utils/Rectangle.hh"
#include "Vendor/Vulkan/Vulkan.Pipeline.hh"
#include "Vendor/Vulkan/Vulkan.Texture.hh"

namespace ct
{
	class CellularAutomatonRenderer
	{
	public:
		CellularAutomatonRenderer(Rectangle size);

		void draw();

	private:
		vulkan::Pipeline GameOfLife;
		vulkan::Pipeline Presentation;
		vulkan::Texture Front, Back;

		CellularAutomatonRenderer(Rectangle size, const vulkan::Shader& vertex);
	};
}
