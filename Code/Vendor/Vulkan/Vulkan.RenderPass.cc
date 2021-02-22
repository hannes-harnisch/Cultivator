#include "PCH.hh"
#include "Vulkan.RenderPass.hh"

#include "Vendor/Vulkan/Vulkan.GraphicsContext.hh"

namespace ct::vulkan
{
	RenderPass::RenderPass()
	{
		std::array attachmenRefs {vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal)};
	}

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

	vk::AttachmentDescription RenderPass::fillAttachmenDescription()
	{
		return vk::AttachmentDescription()
			.setFormat(vk::Format::eB8G8R8A8Snorm)
			.setSamples(vk::SampleCountFlagBits::e1)
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setStoreOp(vk::AttachmentStoreOp::eStore)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setInitialLayout(vk::ImageLayout::eUndefined)
			.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);
	}

	vk::SubpassDescription RenderPass::fillSubpassDescription()
	{
		vk::AttachmentReference attachmentRef(0, vk::ImageLayout::eColorAttachmentOptimal);
		return vk::SubpassDescription();
	}
}
