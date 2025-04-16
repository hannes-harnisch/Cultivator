#include "Buffer.hpp"

#include "graphics/vulkan/Util.hpp"
#include "util/Util.hpp"

namespace cltv {

Buffer::Buffer(const DeviceContext* ctx, size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) :
	ctx_(ctx) {
	VkBufferCreateInfo buffer_info {
		.sType				   = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext				   = nullptr,
		.flags				   = 0,
		.size				   = size,
		.usage				   = usage,
		.sharingMode		   = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices   = nullptr,
	};
	VkResult result = ctx->lib.vkCreateBuffer(ctx->device(), &buffer_info, nullptr, &buffer_);
	require_vk_result(result, "failed to create Vulkan buffer");

	VkMemoryRequirements requirements;
	ctx->lib.vkGetBufferMemoryRequirements(ctx->device(), buffer_, &requirements);

	auto mem_type_index = ctx->find_memory_type_index(requirements.memoryTypeBits, properties);
	require(mem_type_index.has_value(), "no suitable memory type for buffer");

	static constexpr VkDeviceSize MinRecommended = 1048576;

	VkMemoryAllocateInfo alloc_info {
		.sType			 = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext			 = nullptr,
		.allocationSize	 = std::max(requirements.size, MinRecommended),
		.memoryTypeIndex = *mem_type_index,
	};
	result = ctx->lib.vkAllocateMemory(ctx->device(), &alloc_info, nullptr, &memory_);
	require_vk_result(result, "failed to allocate GPU memory for buffer");

	result = ctx->lib.vkBindBufferMemory(ctx->device(), buffer_, memory_, 0);
	require_vk_result(result, "failed to bind buffer memory");
}

Buffer::~Buffer() {
	ctx_->lib.vkDestroyBuffer(ctx_->device(), buffer_, nullptr);
	ctx_->lib.vkFreeMemory(ctx_->device(), memory_, nullptr);
}

} // namespace cltv
