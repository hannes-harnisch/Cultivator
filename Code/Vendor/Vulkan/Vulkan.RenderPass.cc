#include "PCH.hh"

#include "Utils/Assert.hh"
#include "Vulkan.GPUContext.hh"
#include "Vulkan.RenderPass.hh"

namespace ct
{
	RenderPass::RenderPass(vk::ImageLayout initial, vk::ImageLayout final)
	{
		std::array attachments {fillAttachmentDescription(initial, final)};
		std::array attachmentRefs {vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal)};
		std::array subpasses {vk::SubpassDescription().setColorAttachments(attachmentRefs)};
		std::array subpassDependencies {vk::SubpassDependency()
											.setSrcSubpass(VK_SUBPASS_EXTERNAL)
											.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
											.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
											.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)};
		auto renderPassInfo =
			vk::RenderPassCreateInfo().setAttachments(attachments).setSubpasses(subpasses).setDependencies(subpassDependencies);

		auto [res, handle] = GPUContext::device().createRenderPass(renderPassInfo, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create Vulkan render pass.");
		renderPass = handle;
	}

	vk::AttachmentDescription RenderPass::fillAttachmentDescription(vk::ImageLayout initial, vk::ImageLayout final)
	{
		return vk::AttachmentDescription()
			.setFormat(vk::Format::eB8G8R8A8Srgb)
			.setSamples(vk::SampleCountFlagBits::e1)
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setStoreOp(vk::AttachmentStoreOp::eStore)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setInitialLayout(initial)
			.setFinalLayout(final);
	}
}
