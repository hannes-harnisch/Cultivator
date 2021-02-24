#include "PCH.hh"
#include "Vulkan.RenderPass.hh"

#include "Utils/Assert.hh"
#include "Vendor/Vulkan/Vulkan.GraphicsContext.hh"

namespace ct::vulkan
{
	RenderPass::RenderPass()
	{
		std::array attachments {fillAttachmentDescription(vk::ImageLayout::eShaderReadOnlyOptimal,
														  vk::ImageLayout::eColorAttachmentOptimal),
								fillAttachmentDescription(vk::ImageLayout::eColorAttachmentOptimal,
														  vk::ImageLayout::eShaderReadOnlyOptimal)};
		std::array attachmentRefs {vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal)};
		std::array subpasses {vk::SubpassDescription().setColorAttachments(attachmentRefs)};
		std::array subpassDependencies {vk::SubpassDependency()
											.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
											.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
											.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)};

		auto renderPassInfo {vk::RenderPassCreateInfo()
								 .setAttachments(attachments)
								 .setSubpasses(subpasses)
								 .setDependencies(subpassDependencies)};
		auto [result, renderPass] {GraphicsContext::device().createRenderPass(renderPassInfo, nullptr, Loader::get())};
		ctAssertResult(result, "Failed to create Vulkan render pass.");
		RenderPassHandle = renderPass;
	}

	RenderPass::~RenderPass()
	{
		GraphicsContext::device().destroyRenderPass(RenderPassHandle, {}, Loader::get());
	}

	RenderPass::RenderPass(RenderPass&& other) noexcept :
		RenderPassHandle {std::exchange(other.RenderPassHandle, nullptr)}
	{}

	RenderPass& RenderPass::operator=(RenderPass&& other) noexcept
	{
		std::swap(RenderPassHandle, other.RenderPassHandle);
		return *this;
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
