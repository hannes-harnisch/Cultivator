#include "PCH.hh"

#include "Utils/Assert.hh"
#include "Vulkan.GPUContext.hh"
#include "Vulkan.RenderPass.hh"

namespace ct
{
	RenderPass::RenderPass(vk::ImageLayout initial, vk::ImageLayout final)
	{
		vk::AttachmentReference attachmentRef;
		attachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::SubpassDescription subpass;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments	 = &attachmentRef;

		vk::SubpassDependency subpassDependency;
		subpassDependency.srcSubpass	= VK_SUBPASS_EXTERNAL;
		subpassDependency.srcStageMask	= vk::PipelineStageFlagBits::eColorAttachmentOutput;
		subpassDependency.dstStageMask	= vk::PipelineStageFlagBits::eColorAttachmentOutput;
		subpassDependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

		vk::AttachmentDescription attachment;
		attachment.format		  = vk::Format::eB8G8R8A8Srgb;
		attachment.samples		  = vk::SampleCountFlagBits::e1;
		attachment.loadOp		  = vk::AttachmentLoadOp::eDontCare;
		attachment.storeOp		  = vk::AttachmentStoreOp::eStore;
		attachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
		attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		attachment.initialLayout  = initial;
		attachment.finalLayout	  = final;

		vk::RenderPassCreateInfo info;
		info.attachmentCount = 1;
		info.pAttachments	 = &attachment;
		info.subpassCount	 = 1;
		info.pSubpasses		 = &subpass;
		info.dependencyCount = 1;
		info.pDependencies	 = &subpassDependency;

		auto [res, handle] = GPUContext::device().createRenderPass(info, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create Vulkan render pass.");
		renderPass = handle;
	}
}
