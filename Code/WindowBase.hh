#pragma once

#include "Assert.hh"
#include "Rectangle.hh"
#include "Vendor/Vulkan/Vulkan.SwapChain.hh"

namespace ct
{
	class WindowBase
	{
	public:
		virtual Rectangle getViewport() = 0;
		virtual void show()				= 0;

	protected:
		void* NativeHandle;
		vulkan::SwapChain SwapChain;

		inline WindowBase(void* nativeWindowHandle, Rectangle viewport) :
			NativeHandle(nativeWindowHandle), SwapChain(NativeHandle, viewport)
		{
			ctEnsure(NativeHandle, "Failed to create window.");
		}

		inline WindowBase(WindowBase&& other) noexcept :
			NativeHandle(std::exchange(other.NativeHandle, nullptr)), SwapChain(std::move(other.SwapChain))
		{}

		inline WindowBase& operator=(WindowBase&& other) noexcept
		{
			std::swap(NativeHandle, other.NativeHandle);
			SwapChain = std::move(other.SwapChain);
			return *this;
		}
	};
}
