#include "PCH.hh"

#include "Vulkan.FrameBuffer.hh"
#include "Vulkan.GPUContext.hh"

namespace ct
{
	FrameBuffer::FrameBuffer(Rectangle const size, RenderPass const& renderPass, vk::ImageView attachment)
	{
		vk::FramebufferCreateInfo info;
		info.renderPass		 = renderPass.handle();
		info.attachmentCount = 1;
		info.pAttachments	 = &attachment;
		info.width			 = size.width;
		info.height			 = size.height;
		info.layers			 = 1;

		auto [res, handle] = GPUContext::device().createFramebuffer(info, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create Vulkan frame buffer.");
		frameBuffer = handle;
	}
}
