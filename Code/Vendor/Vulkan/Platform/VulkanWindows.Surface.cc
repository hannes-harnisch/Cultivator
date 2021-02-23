#include "PCH.hh"
#include "../Vulkan.Surface.hh"

#include "Utils/Assert.hh"
#include "Vendor/Windows/Windows.AppContext.hh"

namespace ct::vulkan
{
	Surface::Surface(void* nativeWindowHandle)
	{
		auto surfaceInfo {vk::Win32SurfaceCreateInfoKHR()
							  .setHinstance(windows::AppContext::nativeInstanceHandle())
							  .setHwnd(static_cast<HWND>(nativeWindowHandle))};
		auto [result, surface] {
			GraphicsContext::instance().createWin32SurfaceKHR(surfaceInfo, nullptr, Loader::getDeviceless())};
		ctEnsureResult(result, "Could not create Vulkan surface for Windows.");
		SurfaceHandle = surface;
	}

	Surface Surface::makeDummy()
	{
		HWND handle {::CreateWindow(windows::AppContext::WindowClassName, L"", 0, 0, 0, 0, 0, nullptr, nullptr,
									windows::AppContext::nativeInstanceHandle(), nullptr)};
		Surface dummy(handle);
		::DestroyWindow(handle);
		return dummy;
	}
}
