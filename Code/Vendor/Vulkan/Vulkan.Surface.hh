#pragma once

#include "PCH.hh"
#include "Vendor/Vulkan/Vulkan.GraphicsContext.hh"

namespace ct::vulkan
{
	class Surface final
	{
	public:
		static Surface makeDummy();

		Surface(void* nativeWindowHandle);

		inline Surface(Surface&& other) noexcept : SurfaceHandle {std::exchange(other.SurfaceHandle, nullptr)}
		{}

		inline ~Surface()
		{
			GraphicsContext::instance().destroySurfaceKHR(SurfaceHandle, {}, Loader::getDeviceless());
		}

		inline Surface& operator=(Surface&& other) noexcept
		{
			std::swap(SurfaceHandle, other.SurfaceHandle);
			return *this;
		}

		inline vk::SurfaceKHR handle() const
		{
			return SurfaceHandle;
		}

	private:
		vk::SurfaceKHR SurfaceHandle;
	};
}
