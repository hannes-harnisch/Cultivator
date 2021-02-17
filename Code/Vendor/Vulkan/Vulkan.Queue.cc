#include "PCH.hh"
#include "Vulkan.Queue.hh"

#include "Assert.hh"
#include "Vendor/Vulkan/Vulkan.GraphicsPlatform.hh"
#include "Vendor/Vulkan/Vulkan.Surface.hh"

namespace ct::vulkan
{
	Queue::Queue(uint32_t familyIndex) :
		FamilyIndex {familyIndex}, QueueHandle {GraphicsPlatform::get().device().getQueue(FamilyIndex, 0)}
	{}

	bool Queue::supportsSurface(const Surface& surface) const
	{
		auto support {GraphicsPlatform::get().adapter().getSurfaceSupportKHR(FamilyIndex, surface.handle())};
		ctEnsureResult(support.result, "Failed to query for Vulkan surface support.");
		return support.value;
	}
}
