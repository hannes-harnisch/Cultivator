#pragma once

#if CT_SYSTEM_WINDOWS
	#include "Vendor/Windows/Windows.Window.hh"
#endif

namespace ct
{
	using Window = CT_SYSTEM::Window;
}
