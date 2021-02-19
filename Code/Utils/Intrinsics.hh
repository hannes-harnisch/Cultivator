#pragma once

#if __clang__
	#define CT_COMPILER_CLANG __clang__
#elif _MSC_VER
	#define CT_COMPILER_MSVC _MSC_VER
#elif __GNUC__
	#define CT_COMPILER_GCC __GNUC__
#endif

#if CT_COMPILER_CLANG

	#define ctDebugBreak() __builtin_debugtrap()

#elif CT_COMPILER_MSVC

	#define ctDebugBreak() __debugbreak()

#elif CT_COMPILER_GCC

	#include <csignal>
	#define ctDebugBreak() std::raise(SIGTRAP)

#endif
