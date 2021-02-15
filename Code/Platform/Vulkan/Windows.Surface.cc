#include "PCH.hh"
#include "Windows.Surface.hh"

#include "Assert.hh"

namespace ct::vulkan::windows
{
	Surface::Surface()
	{
		HWND windowHandle = ::CreateWindow(TEXT(CT_APP_NAME), L"", 0, 0, 0, 0, 0, nullptr, nullptr, nullptr, nullptr);
		initializeSurfaceHandle(windowHandle);
	}

	Surface::Surface(void* nativeWindowHandle)
	{
		initializeSurfaceHandle(static_cast<HWND>(nativeWindowHandle));
	}

	void Surface::initializeSurfaceHandle(HWND windowHandle)
	{
		auto info {vk::Win32SurfaceCreateInfoKHR().setHwnd(windowHandle)};
		SurfaceHandle = GraphicsPlatform::get().instance().createWin32SurfaceKHR(info);
		ctEnsure(SurfaceHandle, "Could not create Vulkan surface for Windows.");
	}
}
