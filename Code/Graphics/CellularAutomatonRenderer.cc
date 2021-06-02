#include "PCH.hh"

#include "CellularAutomatonRenderer.hh"

namespace ct
{
	CellularAutomatonRenderer::CellularAutomatonRenderer(Rectangle size) :
		CellularAutomatonRenderer(size, vulkan::Shader("ScreenQuad.vert.spv"))
	{}

	void CellularAutomatonRenderer::draw()
	{}

	CellularAutomatonRenderer::CellularAutomatonRenderer(Rectangle size, vulkan::Shader const& vertex) :
		GameOfLife(vertex, vulkan::Shader("GameOfLife.frag.spv")),
		Presentation(vertex, vulkan::Shader("Presentation.frag.spv")),
		Front(size),
		Back(size)
	{}
}
