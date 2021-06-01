#pragma once

#include "Utils/Assert.hh"

namespace ct
{
	template<typename T> class Singleton
	{
	protected:
		static inline T* SingletonInstance;

		Singleton()
		{
			ctEnsure(!SingletonInstance, "This type can only be instantiated once.");
			SingletonInstance = static_cast<T*>(this);
		}
	};
}
