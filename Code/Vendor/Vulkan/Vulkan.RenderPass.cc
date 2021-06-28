#include "PCH.hh"

#include "Utils/Assert.hh"
#include "Vulkan.GPUContext.hh"
#include "Vulkan.RenderPass.hh"

namespace ct
{
	RenderPass::RenderPass(vk::ImageLayout initial, vk::ImageLayout final)
	{
		vk::AttachmentReference colorAttach {
			.attachment = 0,
			.layout		= vk::ImageLayout::eColorAttachmentOptimal,
		};
		vk::SubpassDescription subpass {
			.colorAttachmentCount = 1,
			.pColorAttachments	  = &colorAttach,
		};
		vk::SubpassDependency subpassDependency {
			.srcSubpass	   = VK_SUBPASS_EXTERNAL,
			.srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput,
			.dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput,
			.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite,
		};
		vk::AttachmentDescription attachment {
			.format			= vk::Format::eB8G8R8A8Srgb,
			.samples		= vk::SampleCountFlagBits::e1,
			.loadOp			= vk::AttachmentLoadOp::eDontCare,
			.storeOp		= vk::AttachmentStoreOp::eStore,
			.stencilLoadOp	= vk::AttachmentLoadOp::eDontCare,
			.stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
			.initialLayout	= initial,
			.finalLayout	= final,
		};
		vk::RenderPassCreateInfo info {
			.attachmentCount = 1,
			.pAttachments	 = &attachment,
			.subpassCount	 = 1,
			.pSubpasses		 = &subpass,
			.dependencyCount = 1,
			.pDependencies	 = &subpassDependency,
		};
		auto [res, handle] = GPUContext::device().createRenderPass(info);
		ctAssertResult(res, "Failed to create Vulkan render pass.");
		renderPass = handle;
	}
}
