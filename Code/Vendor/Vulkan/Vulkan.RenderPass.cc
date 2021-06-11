#include "PCH.hh"

#include "Utils/Assert.hh"
#include "Vulkan.GPUContext.hh"
#include "Vulkan.RenderPass.hh"

namespace ct
{
	RenderPass::RenderPass(vk::ImageLayout initial, vk::ImageLayout final)
	{
		vk::AttachmentReference attachmentRef(0, vk::ImageLayout::eColorAttachmentOptimal);
		vk::SubpassDescription subpass;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments	 = &attachmentRef;

		vk::SubpassDependency subpassDependency;
		subpassDependency.srcSubpass	= VK_SUBPASS_EXTERNAL;
		subpassDependency.srcStageMask	= vk::PipelineStageFlagBits::eColorAttachmentOutput;
		subpassDependency.dstStageMask	= vk::PipelineStageFlagBits::eColorAttachmentOutput;
		subpassDependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

		auto attachmentDesc = fillAttachmentDescription(initial, final);

		vk::RenderPassCreateInfo info;
		info.attachmentCount = 1;
		info.pAttachments	 = &attachmentDesc;
		info.subpassCount	 = 1;
		info.pSubpasses		 = &subpass;
		info.dependencyCount = 1;
		info.pDependencies	 = &subpassDependency;

		auto [res, handle] = GPUContext::device().createRenderPass(info, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create Vulkan render pass.");
		renderPass = handle;
	}

	vk::AttachmentDescription RenderPass::fillAttachmentDescription(vk::ImageLayout initial, vk::ImageLayout final)
	{
		vk::AttachmentDescription desc;
		desc.format			= vk::Format::eB8G8R8A8Srgb;
		desc.samples		= vk::SampleCountFlagBits::e1;
		desc.loadOp			= vk::AttachmentLoadOp::eClear;
		desc.storeOp		= vk::AttachmentStoreOp::eStore;
		desc.stencilLoadOp	= vk::AttachmentLoadOp::eDontCare;
		desc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		desc.initialLayout	= initial;
		desc.finalLayout	= final;
		return desc;
	}
}
