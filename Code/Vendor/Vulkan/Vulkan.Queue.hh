#pragma once

#include "PCH.hh"

namespace ct
{
	class Surface;

	class Queue final
	{
	public:
		Queue() = default;
		Queue(uint32_t familyIndex);

		bool supportsSurface(Surface const& surface) const;
		void submit(vk::CommandBuffer commandBuffer, vk::Semaphore imgGet, vk::Semaphore imgDone, vk::Fence fence);
		void submitSync(vk::CommandBuffer commandBuffer);

		uint32_t getFamily() const
		{
			return familyIndex;
		}

		vk::Queue handle() const
		{
			return queue;
		}

	private:
		uint32_t familyIndex = UINT32_MAX;
		vk::Queue queue;
	};
}
