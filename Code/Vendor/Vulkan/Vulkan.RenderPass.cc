#include "PCH.hh"
#include "Vulkan.RenderPass.hh"

#include "Vendor/Vulkan/Vulkan.GraphicsContext.hh"

namespace ct::vulkan
{
	RenderPass::~RenderPass()
	{
		GraphicsContext::device().destroyRenderPass(Pass);
	}

	RenderPass::RenderPass(RenderPass&& other) noexcept : Pass {std::exchange(other.Pass, nullptr)}
	{}

	RenderPass& RenderPass::operator=(RenderPass&& other) noexcept
	{
		std::swap(Pass, other.Pass);
		return *this;
	}
}
