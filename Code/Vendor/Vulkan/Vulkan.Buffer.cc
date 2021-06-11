#include "PCH.hh"

#include "Vulkan.Buffer.hh"
#include "Vulkan.Utils.hh"

namespace ct
{
	Buffer::Buffer(size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties)
	{
		vk::BufferCreateInfo bufferInfo;
		bufferInfo.size	 = size;
		bufferInfo.usage = usage;

		auto [res, bufHandle] = GPUContext::device().createBuffer(bufferInfo, nullptr, Loader::get());
		ctEnsureResult(res, "Failed to create buffer.");
		buf = bufHandle;

		auto memoryRequirements = GPUContext::device().getBufferMemoryRequirements(buf, Loader::get());

		vk::MemoryAllocateInfo allocInfo;
		allocInfo.allocationSize  = memoryRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, properties);

		auto [allocRes, memHandle] = GPUContext::device().allocateMemory(allocInfo, nullptr, Loader::get());
		ctEnsureResult(allocRes, "Failed to allocate Vulkan memory.");
		mem = memHandle;

		GPUContext::device().bindBufferMemory(buf, mem, 0, Loader::get());
	}
}
