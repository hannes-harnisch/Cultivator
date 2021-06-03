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

		Surface(Surface&& other) noexcept : surface(std::exchange(other.surface, nullptr))
		{}

		~Surface()
		{
			GPUContext::instance().destroySurfaceKHR(surface, {}, Loader::get());
		}

		Surface& operator=(Surface&& other) noexcept
		{
			std::swap(surface, other.surface);
			return *this;
		}

		vk::SurfaceKHR handle() const
		{
			return surface;
		}

	private:
		vk::SurfaceKHR surface;
	};
}
