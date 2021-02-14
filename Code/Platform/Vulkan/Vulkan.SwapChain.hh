#pragma once

#include "PCH.hh"

namespace ct::vulkan
{
	class SwapChain
	{
	public:
		SwapChain() = default;
		SwapChain(void* nativeWindowHandle);
		SwapChain(SwapChain&& other) noexcept;
		~SwapChain();
		SwapChain& operator=(SwapChain&& other) noexcept;

		SwapChain(const SwapChain&) = delete;
		SwapChain& operator=(const SwapChain&) = delete;

	private:
		vk::SurfaceKHR Surface;
		vk::SwapchainKHR Chain;
	};
}
