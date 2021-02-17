#include "PCH.hh"
#include "../Vulkan.Surface.hh"

#include "Assert.hh"

namespace ct::vulkan
{
	Surface::Surface(void* nativeWindowHandle)
	{
		HWND handle {static_cast<HWND>(nativeWindowHandle)};
		auto surfaceInfo {vk::Win32SurfaceCreateInfoKHR().setHwnd(handle)};
		auto surface {GraphicsPlatform::get().instance().createWin32SurfaceKHR(surfaceInfo)};
		ctEnsureResult(surface.result, "Could not create Vulkan surface for Windows.");
		SurfaceHandle = surface.value;
	}

	Surface Surface::makeDummy()
	{
		HWND handle {::CreateWindow(TEXT(CT_APP_NAME), L"", 0, 0, 0, 0, 0, nullptr, nullptr, nullptr, nullptr)};
		Surface dummy(handle);
		::DestroyWindow(handle);
		return dummy;
	}
}
