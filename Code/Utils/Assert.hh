#pragma once

#include "PCH.hh"

#include "Utils/Intrinsics.hh"

namespace ct
{
	void showErrorBox(const std::string& title, const std::string& message);

	inline void crash(const std::string& message)
	{
		showErrorBox("FATAL ERROR", message);
		std::exit(EXIT_FAILURE);
	}
}

#if CT_DEBUG

	#define ctAssert(condition, message)                                                                                       \
		{                                                                                                                      \
			if(!(condition))                                                                                                   \
				ctDebugBreak();                                                                                                \
		}

	#define ctAssertResult(condition, message) ctAssert(static_cast<int64_t>(condition) >= 0, message)
	#define ctAssertPure(condition, message)   ctAssert(condition, message)

	#define ctEnsure(condition, message)	   ctAssert(condition, message)
	#define ctEnsureResult(condition, message) ctAssertResult(condition, message)

#else

	#define ctAssert(condition, message)	   static_cast<void>(condition)
	#define ctAssertResult(condition, message) static_cast<void>(condition)
	#define ctAssertPure(condition, message)

	#define ctEnsure(condition, message)                                                                                       \
		{                                                                                                                      \
			if(!(condition))                                                                                                   \
				ct::crash(message);                                                                                            \
		}

	#define ctEnsureResult(condition, message) ctEnsure(static_cast<int64_t>(condition) >= 0, message)

#endif
