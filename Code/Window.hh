#pragma once

#include "Platform/Windows/Windows.Window.hh"

namespace ct
{
#if CT_SYSTEM_WINDOWS
	typedef windows::Window Window;
#endif
}
