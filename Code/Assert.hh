#pragma once

#include "Intrinsics.hh"
#include <cstdlib>
#include <string>

namespace ct
{
	void showErrorBox(const std::string& title, const std::string& message);

	inline void crash(const std::string& message)
	{
		showErrorBox("FATAL ERROR", message);
		std::exit(EXIT_FAILURE);
	}

	template<typename T, uint32_t N> constexpr uint32_t countOf(T (&)[N])
	{
		return N;
	}
}

#if CT_DEBUG

	#define ctAssert(condition, message)                                                                               \
		{                                                                                                              \
			if(!(condition))                                                                                           \
				ctDebugBreak();                                                                                        \
		}

	#define ctAssertResult(condition, message) ctAssert((condition) >= 0, message)
	#define ctAssertPure(condition, message)   ctAssert(condition, message)

	#define ctEnsure(condition, message)	   ctAssert(condition, message)
	#define ctEnsureResult(condition, message) ctAssertResult(condition, message)

#else

	#define ctAssert(condition, message)	   void(condition)
	#define ctAssertResult(condition, message) void(condition)
	#define ctAssertPure(condition, message)

	#define ctEnsure(condition, message)                                                                               \
		{                                                                                                              \
			if(!(condition))                                                                                           \
				ct::crash(message);                                                                                    \
		}

	#define ctEnsureResult(condition, message) ctEnsure((condition) >= 0, message)

#endif
