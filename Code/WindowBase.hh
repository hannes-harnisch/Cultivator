#pragma once

#include "Vendor/Vulkan/Vulkan.SwapChain.hh"

namespace ct
{
	class WindowBase
	{
	public:
		virtual void show() = 0;

	protected:
		vulkan::SwapChain SwapChain;
	};
}
