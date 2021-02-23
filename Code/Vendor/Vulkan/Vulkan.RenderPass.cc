#include "PCH.hh"
#include "Vulkan.RenderPass.hh"

#include "Vendor/Vulkan/Vulkan.GraphicsContext.hh"

namespace ct::vulkan
{
	RenderPass::RenderPass()
	{
		std::array attachmentRefs {vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal)};
	}

	RenderPass::~RenderPass()
	{
		GraphicsContext::device().destroyRenderPass(Pass, {}, Loader::get());
	}

	RenderPass::RenderPass(RenderPass&& other) noexcept : Pass {std::exchange(other.Pass, nullptr)}
	{}

	RenderPass& RenderPass::operator=(RenderPass&& other) noexcept
	{
		std::swap(Pass, other.Pass);
		return *this;
	}

	vk::AttachmentDescription RenderPass::fillAttachmentDescription(vk::Format format)
	{
		return vk::AttachmentDescription()
			.setFormat(format)
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
		return vk::SubpassDescription();
	}
}
