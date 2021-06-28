#include "PCH.hh"

#include "Vulkan.FrameBuffer.hh"
#include "Vulkan.GPUContext.hh"

namespace ct
{
	FrameBuffer::FrameBuffer(Rectangle const size, RenderPass const& renderPass, vk::ImageView attachment)
	{
		vk::FramebufferCreateInfo info {
			.renderPass		 = renderPass.handle(),
			.attachmentCount = 1,
			.pAttachments	 = &attachment,
			.width			 = size.width,
			.height			 = size.height,
			.layers			 = 1,
		};
		auto [res, handle] = GPUContext::device().createFramebuffer(info);
		ctAssertResult(res, "Failed to create Vulkan frame buffer.");
		frameBuffer = handle;
	}
}
