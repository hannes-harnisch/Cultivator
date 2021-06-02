#pragma once

#include "PCH.hh"

#include "Vendor/Vulkan/Vulkan.GPUContext.hh"

namespace ct::vulkan
{
	class Surface final
	{
	public:
		static Surface makeDummy();

		Surface(void* nativeWindowHandle);

		Surface(Surface&& other) noexcept : SurfaceHandle(std::exchange(other.SurfaceHandle, nullptr))
		{}

		~Surface()
		{
			GPUContext::instance().destroySurfaceKHR(SurfaceHandle, {}, Loader::get());
		}

		Surface& operator=(Surface&& other) noexcept
		{
			std::swap(SurfaceHandle, other.SurfaceHandle);
			return *this;
		}

		vk::SurfaceKHR handle() const
		{
			return SurfaceHandle;
		}

	private:
		vk::SurfaceKHR SurfaceHandle;
	};
}
