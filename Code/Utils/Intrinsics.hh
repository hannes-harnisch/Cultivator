#pragma once

#if __clang__
	#define CT_COMPILER_CLANG __clang__
#elif _MSC_VER
	#define CT_COMPILER_MSVC _MSC_VER
#elif __GNUC__
	#define CT_COMPILER_GCC __GNUC__
#endif

#if CT_COMPILER_CLANG

	#define CT_FUNCTION_NAME __PRETTY_FUNCTION__
	#define CT_DLLEXPORT	 __declspec(dllexport)
	#define CT_DLLIMPORT	 __declspec(dllimport)
	#define CT_INLINE		 __attribute__((always_inline))
	#define CT_RESTRICT		 __restrict__

	#define ctDebugBreak() __builtin_debugtrap()

#elif CT_COMPILER_MSVC

	#define CT_FUNCTION_NAME __FUNCSIG__
	#define CT_DLLEXPORT	 __declspec(dllexport)
	#define CT_DLLIMPORT	 __declspec(dllimport)
	#define CT_INLINE		 __forceinline
	#define CT_RESTRICT		 __restrict

	#define ctDebugBreak() __debugbreak()

#elif CT_COMPILER_GCC

	#define CT_FUNCTION_NAME __PRETTY_FUNCTION__
	#define CT_DLLEXPORT	 __attribute__((dllexport))
	#define CT_DLLIMPORT	 __attribute__((dllimport))
	#define CT_INLINE		 __attribute__((always_inline)) inline
	#define CT_RESTRICT		 __restrict__

	#include <csignal>
	#define ctDebugBreak() std::raise(SIGTRAP)

#endif
