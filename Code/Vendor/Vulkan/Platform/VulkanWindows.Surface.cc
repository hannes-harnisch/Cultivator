#include "PCH.hh"

#include "../Vulkan.Surface.hh"
#include "Utils/Assert.hh"
#include "Vendor/Windows/Windows.AppContext.hh"

namespace ct
{
	Surface::Surface(void* nativeWindowHandle)
	{
		vk::Win32SurfaceCreateInfoKHR info {
			.hinstance = windows::AppContext::nativeInstanceHandle(),
			.hwnd	   = static_cast<HWND>(nativeWindowHandle),
		};
		auto [res, handle] = GPUContext::instance().createWin32SurfaceKHR(info);
		ctEnsureResult(res, "Could not create Vulkan surface for Windows.");
		surface = handle;
	}

	Surface Surface::makeDummy()
	{
		HWND handle = ::CreateWindow(windows::AppContext::WindowClassName, L"", 0, 0, 0, 0, 0, nullptr, nullptr,
									 windows::AppContext::nativeInstanceHandle(), nullptr);
		Surface dummy(handle);
		::DestroyWindow(handle);
		return dummy;
	}
}
