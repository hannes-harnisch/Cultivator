#pragma once

#include "Vulkan.Unique.hh"

namespace ct
{
	class Buffer
	{
	public:
		Buffer(size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);

		vk::Buffer buffer() const
		{
			return buf;
		}

		vk::DeviceMemory memory() const
		{
			return mem;
		}

	private:
		DeviceOwn<vk::DeviceMemory, &vk::Device::freeMemory> mem;
		DeviceOwn<vk::Buffer, &vk::Device::destroyBuffer> buf;
	};
}
