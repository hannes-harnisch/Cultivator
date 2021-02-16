#pragma once

#include "AppPlatformBase.hh"

namespace ct::windows
{
	class AppPlatform : public AppPlatformBase
	{
	public:
		AppPlatform();

		void pollEvents() override;
	};
}
