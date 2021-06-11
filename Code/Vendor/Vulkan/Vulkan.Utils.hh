#pragma once

#include "PCH.hh"

#include "Utils/Assert.hh"
#include "Vulkan.GPUContext.hh"

namespace ct
{
	uint32_t count(auto const& container)
	{
		return static_cast<uint32_t>(std::size(container));
	}

	inline uint32_t findMemoryType(uint32_t filter, vk::MemoryPropertyFlags memProperties)
	{
		auto memProps = GPUContext::adapter().getMemoryProperties(Loader::get());

		for(uint32_t i {}; i < memProps.memoryTypeCount; ++i)
		{
			bool hasFlag = (memProps.memoryTypes[i].propertyFlags & memProperties) == memProperties;
			if(filter & (1 << i) && hasFlag)
				return i;
		}
		throw "Failed to find Vulkan memory type.";
	}
}
