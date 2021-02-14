#include "PCH.hh"
#include "Vulkan.SwapChain.hh"

#include "Assert.hh"
#include "Platform/Vulkan/Vulkan.Adapter.hh"

namespace ct::vulkan
{
	SwapChain::SwapChain(void* nativeWindowHandle)
	{
		auto info {vk::Win32SurfaceCreateInfoKHR().setHwnd(static_cast<HWND>(nativeWindowHandle))};
		Surface = Adapter::get().instance().createWin32SurfaceKHR(info);
		ctEnsure(Surface, "Could not create Vulkan surface for Windows.");
	}

	SwapChain::SwapChain(SwapChain&& other) noexcept
	{
		*this = std::move(other);
	}

	SwapChain::~SwapChain()
	{
		if(Surface)
			Adapter::get().instance().destroySurfaceKHR(Surface);
	}

	SwapChain& SwapChain::operator=(SwapChain&& other) noexcept
	{
		std::swap(Surface, other.Surface);
		std::swap(Chain, other.Chain);
		return *this;
	}

}
