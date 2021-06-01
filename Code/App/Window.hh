#pragma once

#if CT_SYSTEM_WINDOWS
	#include "Vendor/Windows/Windows.Window.hh"
#endif

namespace ct
{
#if CT_SYSTEM_WINDOWS
	using Window = windows::Window;
#endif
}
