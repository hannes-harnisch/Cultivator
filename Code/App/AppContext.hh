#pragma once

#if CT_SYSTEM_WINDOWS
	#include "Vendor/Windows/Windows.AppContext.hh"
#endif

namespace ct
{
#if CT_SYSTEM_WINDOWS
	using AppContext = windows::AppContext;
#endif
}
