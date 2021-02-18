#pragma once

namespace ct
{
	class AppPlatformBase
	{
	public:
		virtual void pollEvents() = 0;
	};
}
