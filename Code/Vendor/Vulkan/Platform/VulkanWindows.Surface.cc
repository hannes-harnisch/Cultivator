#include "PCH.hh"

#include "../Vulkan.Surface.hh"
#include "Utils/Assert.hh"
#include "Vendor/Windows/Windows.AppContext.hh"

namespace ct::vulkan
{
	Surface::Surface(void* nativeWindowHandle)
	{
		auto info {vk::Win32SurfaceCreateInfoKHR()
					   .setHinstance(windows::AppContext::nativeInstanceHandle())
					   .setHwnd(static_cast<HWND>(nativeWindowHandle))};
		auto [res, surface] {GPUContext::instance().createWin32SurfaceKHR(info, nullptr, Loader::getDeviceless())};
		ctEnsureResult(res, "Could not create Vulkan surface for Windows.");
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
