#include "PCH.hh"
#include "Vulkan.Queue.hh"

#include "Utils/Assert.hh"
#include "Vendor/Vulkan/Vulkan.GPUContext.hh"
#include "Vendor/Vulkan/Vulkan.Surface.hh"

namespace ct::vulkan
{
	Queue::Queue(uint32_t familyIndex) :
		FamilyIndex {familyIndex}, QueueHandle {GPUContext::device().getQueue(FamilyIndex, 0, Loader::get())}
	{}

	bool Queue::supportsSurface(const Surface& surface) const
	{
		auto handle {surface.handle()};
		auto [res, supports] {GPUContext::adapter().getSurfaceSupportKHR(FamilyIndex, handle, Loader::get())};
		ctEnsureResult(res, "Failed to query for Vulkan surface support.");
		return supports;
	}
}
