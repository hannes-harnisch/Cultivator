#pragma once

#include "PCH.hh"
#include "Vendor/Vulkan/Vulkan.GraphicsPlatform.hh"

namespace ct::vulkan
{
	class Surface
	{
	public:
		Surface() = default;
		Surface(void* nativeWindowHandle);

		inline Surface(Surface&& other) noexcept
		{
			*this = std::move(other);
		}

		inline ~Surface()
		{
			if(SurfaceHandle)
				GraphicsPlatform::get().instance().destroySurfaceKHR(SurfaceHandle);
		}

		inline Surface& operator=(Surface&& other) noexcept
		{
			std::swap(SurfaceHandle, other.SurfaceHandle);
			return *this;
		}

		static Surface makeDummy();

		inline vk::SurfaceKHR handle()
		{
			return SurfaceHandle;
		}

		Surface(const Surface&) = delete;
		Surface& operator=(const Surface&) = delete;

	private:
		vk::SurfaceKHR SurfaceHandle;
	};
}
