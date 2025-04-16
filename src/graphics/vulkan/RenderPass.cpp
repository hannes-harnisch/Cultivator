#include "RenderPass.hpp"

#include "graphics/vulkan/Util.hpp"

namespace cltv {

RenderPass::RenderPass(const DeviceContext* ctx, VkImageLayout initial, VkImageLayout final) :
	ctx_(ctx) {
	VkAttachmentReference color_attachment {
		.attachment = 0,
		.layout		= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	};
	VkSubpassDescription subpass {
		.flags					 = 0,
		.pipelineBindPoint		 = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.inputAttachmentCount	 = 0,
		.pInputAttachments		 = nullptr,
		.colorAttachmentCount	 = 1,
		.pColorAttachments		 = &color_attachment,
		.pResolveAttachments	 = nullptr,
		.pDepthStencilAttachment = nullptr,
		.preserveAttachmentCount = 0,
		.pPreserveAttachments	 = nullptr,
	};
	VkSubpassDependency subpass_dependency {
		.srcSubpass		 = VK_SUBPASS_EXTERNAL,
		.dstSubpass		 = 0,
		.srcStageMask	 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstStageMask	 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask	 = 0,
		.dstAccessMask	 = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.dependencyFlags = 0,
	};
	VkAttachmentDescription attachment {
		.flags			= 0,
		.format			= VK_FORMAT_B8G8R8A8_SRGB,
		.samples		= VK_SAMPLE_COUNT_1_BIT,
		.loadOp			= VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.storeOp		= VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout	= initial,
		.finalLayout	= final,
	};
	VkRenderPassCreateInfo render_pass_info {
		.sType			 = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.pNext			 = nullptr,
		.flags			 = 0,
		.attachmentCount = 1,
		.pAttachments	 = &attachment,
		.subpassCount	 = 1,
		.pSubpasses		 = &subpass,
		.dependencyCount = 1,
		.pDependencies	 = &subpass_dependency,
	};
	VkResult result = ctx->lib.vkCreateRenderPass(ctx->device(), &render_pass_info, nullptr, &render_pass_);
	require_vk_result(result, "failed to create Vulkan render pass");
}

RenderPass::~RenderPass() {
	ctx_->lib.vkDestroyRenderPass(ctx_->device(), render_pass_, nullptr);
}

} // namespace cltv
