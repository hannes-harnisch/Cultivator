#pragma once

#if CT_SYSTEM_WINDOWS
	#include "Platform/Windows/Windows.Window.hh"
#endif

namespace ct
{
#if CT_SYSTEM_WINDOWS
	using Window = ct::windows::Window;
#endif
}
