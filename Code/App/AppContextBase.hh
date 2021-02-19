#pragma once

namespace ct
{
	class AppContextBase
	{
	public:
		virtual void pollEvents() = 0;
	};
}
