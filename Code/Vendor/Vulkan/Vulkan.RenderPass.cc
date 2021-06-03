#include "PCH.hh"

#include "Utils/Assert.hh"
#include "Vendor/Vulkan/Vulkan.GPUContext.hh"
#include "Vulkan.RenderPass.hh"

namespace ct::vulkan
{
	RenderPass::RenderPass() : renderPass(makeRenderPass())
	{}

	vk::RenderPass RenderPass::makeRenderPass()
	{
		std::array attachments {
			fillAttachmentDescription(vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eShaderReadOnlyOptimal)};
		std::array attachmentRefs {vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal)};
		std::array subpasses {vk::SubpassDescription().setColorAttachments(attachmentRefs)};
		std::array subpassDependencies {vk::SubpassDependency()
											.setSrcSubpass(VK_SUBPASS_EXTERNAL)
											.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
											.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
											.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)};
		auto renderPassInfo =
			vk::RenderPassCreateInfo().setAttachments(attachments).setSubpasses(subpasses).setDependencies(subpassDependencies);

		auto [res, renderPass] = GPUContext::device().createRenderPass(renderPassInfo, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create Vulkan render pass.");
		return renderPass;
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
