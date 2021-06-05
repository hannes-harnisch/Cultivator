#include "PCH.hh"

#include "Vendor/Vulkan/Vulkan.GPUContext.hh"
#include "Vulkan.FrameBuffer.hh"

namespace ct
{
	FrameBuffer::FrameBuffer(Rectangle const size, RenderPass const& renderPass) :
		frameBuffer(createFrameBuffer(size, renderPass))
	{}

	vk::Framebuffer FrameBuffer::createFrameBuffer(Rectangle const size, RenderPass const& renderPass)
	{
		auto frameBufferInfo = vk::FramebufferCreateInfo()
								   .setRenderPass(renderPass.handle())
								   //.setAttachments() TODO
								   .setWidth(size.width)
								   .setHeight(size.height)
								   .setLayers(1);
		auto [res, frameBuffer] = GPUContext::device().createFramebuffer(frameBufferInfo, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create Vulkan frame buffer.");
		return frameBuffer;
	}
}
