#pragma once

#include "PCH.hh"
#include "Platform/Vulkan/Vulkan.GraphicsPlatform.hh"

namespace ct::vulkan
{
	class SurfaceBase
	{
	public:
		inline SurfaceBase(SurfaceBase&& other) noexcept
		{
			*this = std::move(other);
		}

		inline ~SurfaceBase()
		{
			if(SurfaceHandle)
				GraphicsPlatform::get().instance().destroySurfaceKHR(SurfaceHandle);
		}

		inline SurfaceBase& operator=(SurfaceBase&& other) noexcept
		{
			std::swap(SurfaceHandle, other.SurfaceHandle);
			return *this;
		}

		SurfaceBase(const SurfaceBase&) = delete;
		SurfaceBase& operator=(const SurfaceBase&) = delete;

	protected:
		vk::SurfaceKHR SurfaceHandle;

		SurfaceBase() = default;
	};
}
