#include "PCH.hh"

#include "Vulkan.FrameBuffer.hh"
#include "Vulkan.GPUContext.hh"

namespace ct
{
	FrameBuffer::FrameBuffer(Rectangle const size, RenderPass const& renderPass, vk::ImageView attachment)
	{
		auto frameBufferInfo = vk::FramebufferCreateInfo()
								   .setRenderPass(renderPass.handle())
								   .setAttachments(attachment)
								   .setWidth(size.width)
								   .setHeight(size.height)
								   .setLayers(1);
		auto [res, handle] = GPUContext::device().createFramebuffer(frameBufferInfo, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create Vulkan frame buffer.");
		frameBuffer = handle;
	}
}
