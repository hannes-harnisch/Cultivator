#pragma once

#include "PCH.hh"
#include "Vendor/Vulkan/Vulkan.Surface.hh"

namespace ct::vulkan
{
	class SwapChain
	{
	public:
		SwapChain() = default;
		SwapChain(void* windowHandle);
		SwapChain(SwapChain&& other) noexcept;
		~SwapChain();
		SwapChain& operator=(SwapChain&& other) noexcept;

		SwapChain(const SwapChain&) = delete;
		SwapChain& operator=(const SwapChain&) = delete;

	private:
		Surface Surface;
		vk::SwapchainKHR SwapChainHandle;
	};
}
