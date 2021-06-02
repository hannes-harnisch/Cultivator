#pragma once

#include "PCH.hh"

namespace ct::vulkan
{
	class Surface;

	class Queue final
	{
	public:
		Queue() = default;
		Queue(uint32_t familyIndex);

		bool supportsSurface(Surface const& surface) const;

		uint32_t familyIndex() const
		{
			return FamilyIndex;
		}

		vk::Queue handle() const
		{
			return QueueHandle;
		}

	private:
		uint32_t FamilyIndex = UINT32_MAX;
		vk::Queue QueueHandle;
	};
}
