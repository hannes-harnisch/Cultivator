#pragma once

#include "PCH.hh"

namespace ct
{
	struct Rectangle
	{
		uint32_t width {}, height {};

		uint32_t area() const
		{
			return width * height;
		}
	};
}
