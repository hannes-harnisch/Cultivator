#pragma once

#include "PCH.hh"
#include "Vendor/Vulkan/Vulkan.GraphicsPlatform.hh"

namespace ct::vulkan
{
	class Surface
	{
	public:
		static Surface makeDummy();

		Surface(void* nativeWindowHandle);

		inline Surface(Surface&& other) noexcept : SurfaceHandle(std::exchange(other.SurfaceHandle, nullptr))
		{}

		inline ~Surface()
		{
			GraphicsPlatform::get().instance().destroySurfaceKHR(SurfaceHandle);
		}

		inline Surface& operator=(Surface&& other) noexcept
		{
			std::swap(SurfaceHandle, other.SurfaceHandle);
			return *this;
		}

		inline vk::SurfaceKHR handle()
		{
			return SurfaceHandle;
		}

	private:
		vk::SurfaceKHR SurfaceHandle;
	};
}
