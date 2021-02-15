#pragma once

#include "Platform/Vulkan/Vulkan.SurfaceBase.hh"

namespace ct::vulkan::windows
{
	class Surface : public SurfaceBase
	{
	public:
		Surface();
		Surface(void* nativeWindowHandle);

	private:
		void initializeSurfaceHandle(HWND windowHandle);
	};
}
