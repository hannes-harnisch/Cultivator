#include "PCH.hh"
#include "Vulkan.Queue.hh"

#include "Utils/Assert.hh"
#include "Vendor/Vulkan/Vulkan.GraphicsContext.hh"
#include "Vendor/Vulkan/Vulkan.Surface.hh"

namespace ct::vulkan
{
	Queue::Queue(uint32_t familyIndex) :
		FamilyIndex {familyIndex}, QueueHandle {GraphicsContext::device().getQueue(FamilyIndex, 0, Loader::get())}
	{}

	bool Queue::supportsSurface(const Surface& surface) const
	{
		auto handle {surface.handle()};
		auto [res, supports] {GraphicsContext::adapter().getSurfaceSupportKHR(FamilyIndex, handle, Loader::get())};
		ctEnsureResult(res, "Failed to query for Vulkan surface support.");
		return supports;
	}
}
