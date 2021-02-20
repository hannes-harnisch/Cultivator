#pragma once

#include "Utils/Assert.hh"
#include "Utils/Rectangle.hh"
#include "Vendor/Vulkan/Vulkan.SwapChain.hh"

namespace ct
{
	class WindowBase
	{
	public:
		virtual Rectangle getViewport() = 0;
		virtual void show()				= 0;

	protected:
		vulkan::SwapChain SwapChain;

		inline WindowBase(void* nativeWindowHandle, Rectangle viewport) : SwapChain {nativeWindowHandle, viewport}
		{
			ctEnsure(nativeWindowHandle, "Failed to create window.");
		}
	};
}
