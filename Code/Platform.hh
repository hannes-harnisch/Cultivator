#pragma once

#if CT_SYSTEM_WINDOWS
	#define CT_SYSTEM windows
#else
	#error Undefined system.
#endif
