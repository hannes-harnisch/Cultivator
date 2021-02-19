#include "PCH.hh"
#include "Vulkan.Queue.hh"

#include "Utils/Assert.hh"
#include "Vendor/Vulkan/Vulkan.GraphicsContext.hh"
#include "Vendor/Vulkan/Vulkan.Surface.hh"

namespace ct::vulkan
{
	Queue::Queue(uint32_t familyIndex) :
		FamilyIndex {familyIndex}, QueueHandle {GraphicsContext::get().device().getQueue(FamilyIndex, 0)}
	{}

	bool Queue::supportsSurface(const Surface& surface) const
	{
		auto [result, supports] {GraphicsContext::get().adapter().getSurfaceSupportKHR(FamilyIndex, surface.handle())};
		ctEnsureResult(result, "Failed to query for Vulkan surface support.");
		return supports;
	}
}
