#pragma once

#include "Utils/Rectangle.hh"

namespace ct
{
	class WindowBase
	{
	public:
		virtual Rectangle getViewport() const = 0;
		virtual void show()					  = 0;
		virtual void* handle() const		  = 0;
	};
}
