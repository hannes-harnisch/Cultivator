#include "PCH.hh"
#include "CellularAutomatonRenderer.hh"

namespace ct
{
	CellularAutomatonRenderer::CellularAutomatonRenderer(Rectangle size) :
		CellularAutomatonRenderer {size, vulkan::Shader {"ScreenQuad.vert.cso"}}
	{}

	CellularAutomatonRenderer::CellularAutomatonRenderer(Rectangle size, const vulkan::Shader& vertex) :
		GameOfLife {vertex, vulkan::Shader {"GameOfLife.frag.cso"}},
		Presentation {vertex, vulkan::Shader {"Presentation.frag.cso"}},
		Front {size},
		Back {size}
	{}
}
