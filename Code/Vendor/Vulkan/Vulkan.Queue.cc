#include "PCH.hh"

#include "Utils/Assert.hh"
#include "Vendor/Vulkan/Vulkan.GPUContext.hh"
#include "Vendor/Vulkan/Vulkan.Surface.hh"
#include "Vulkan.Queue.hh"

namespace ct::vulkan
{
	Queue::Queue(uint32_t familyIndex) :
		FamilyIndex(familyIndex), QueueHandle(GPUContext::device().getQueue(FamilyIndex, 0, Loader::get()))
	{}

	bool Queue::supportsSurface(Surface const& surface) const
	{
		auto handle			 = surface.handle();
		auto [res, supports] = GPUContext::adapter().getSurfaceSupportKHR(FamilyIndex, handle, Loader::get());
		ctEnsureResult(res, "Failed to query for Vulkan surface support.");
		return supports;
	}
}
