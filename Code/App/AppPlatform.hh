#pragma once

#if CT_SYSTEM_WINDOWS
	#include "Vendor/Windows/Windows.AppPlatform.hh"
#endif

namespace ct
{
#if CT_SYSTEM_WINDOWS
	using AppPlatform = windows::AppPlatform;
#endif
}
