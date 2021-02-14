#pragma once

#include "Platform/Windows/Windows.Window.hh"
#include <string>

namespace ct
{
#if CT_SYSTEM_WINDOWS
	typedef windows::Window Window;
#endif
}
