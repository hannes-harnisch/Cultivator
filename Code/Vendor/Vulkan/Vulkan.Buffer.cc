#include "PCH.hh"

#include "Vulkan.Buffer.hh"
#include "Vulkan.Utils.hh"

namespace ct
{
	Buffer::Buffer(size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties)
	{
		vk::BufferCreateInfo bufferInfo {
			.size  = size,
			.usage = usage,
		};
		auto [res, bufHandle] = GPUContext::device().createBuffer(bufferInfo);
		ctAssertResult(res, "Failed to create buffer.");
		buf = bufHandle;

		auto memoryRequirements = GPUContext::device().getBufferMemoryRequirements(buf);

		static constexpr VkDeviceSize MinimumRecommended = 1048576;

		vk::MemoryAllocateInfo allocInfo {
			.allocationSize	 = std::max(memoryRequirements.size, MinimumRecommended),
			.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, properties),
		};
		auto [allocRes, memHandle] = GPUContext::device().allocateMemory(allocInfo);
		ctAssertResult(allocRes, "Failed to allocate Vulkan memory.");
		mem = memHandle;

		ctAssertResult(GPUContext::device().bindBufferMemory(buf, mem, 0), "Failed to bind buffer memory.");
	}
}
