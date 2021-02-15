#include "PCH.hh"
#include "Vulkan.SwapChain.hh"

#include "Assert.hh"
#include "Platform/Vulkan/Vulkan.GraphicsPlatform.hh"

namespace ct::vulkan
{
	SwapChain::SwapChain(void* windowHandle) : Surface(windowHandle)
	{}

	SwapChain::SwapChain(SwapChain&& other) noexcept
	{
		*this = std::move(other);
	}

	SwapChain::~SwapChain()
	{}

	SwapChain& SwapChain::operator=(SwapChain&& other) noexcept
	{
		std::swap(SwapChainHandle, other.SwapChainHandle);
		return *this;
	}
}
