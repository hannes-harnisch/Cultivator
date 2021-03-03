#pragma once

#include "PCH.hh"
#include "Vendor/Vulkan/Vulkan.GPUContext.hh"

namespace ct::vulkan
{
	inline uint32_t findMemoryType(uint32_t filter, vk::MemoryPropertyFlagBits memProperties)
	{
		auto memProps {GPUContext::adapter().getMemoryProperties(Loader::get())};

		for(uint32_t i {}; i < memProps.memoryTypeCount; ++i)
		{
			bool hasFlag {(memProps.memoryTypes[i].propertyFlags & memProperties) == memProperties};
			if(filter & (1 << i) && hasFlag)
				return i;
		}

		ctEnsureResult(false, "Failed to find Vulkan memory type.");
		return 0;
	}
}
