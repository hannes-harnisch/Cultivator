#pragma once

#if CT_SYSTEM_WINDOWS
	#include "Platform/Vulkan/Windows.Surface.hh"
#endif

namespace ct::vulkan
{
#if CT_SYSTEM_WINDOWS
	using Surface = windows::Surface;
#endif
}
