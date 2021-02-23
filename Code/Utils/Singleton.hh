#pragma once

#include "Utils/Assert.hh"

namespace ct
{
	template<typename T> class Singleton
	{
	public:
		inline Singleton()
		{
			ctEnsure(!SingletonPtr, "This type can only be instantiated once.");
			SingletonPtr = this;
		}

	protected:
		static inline T* SingletonPtr;
	};
}
