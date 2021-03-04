#include "PCH.hh"
#include "Vulkan.FrameBuffer.hh"

#include "Vendor/Vulkan/Vulkan.GPUContext.hh"

namespace ct::vulkan
{
	FrameBuffer::FrameBuffer(Rectangle size, const RenderPass& renderPass)
	{
		auto frameBufferInfo {vk::FramebufferCreateInfo()
								  .setRenderPass(renderPass.handle())
								  //.setAttachments() TODO
								  .setWidth(size.Width)
								  .setHeight(size.Height)
								  .setLayers(1)};
		auto [res, frameBuffer] {GPUContext::device().createFramebuffer(frameBufferInfo, nullptr, Loader::get())};
		ctAssertResult(res, "Failed to create Vulkan frame buffer.");
		FrameBufferHandle = frameBuffer;
	}

	FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept :
		FrameBufferHandle {std::exchange(other.FrameBufferHandle, nullptr)}
	{}

	FrameBuffer::~FrameBuffer()
	{
		GPUContext::device().destroyFramebuffer(FrameBufferHandle, {}, Loader::get());
	}

	FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other) noexcept
	{
		std::swap(FrameBufferHandle, other.FrameBufferHandle);
		return *this;
	}
}
