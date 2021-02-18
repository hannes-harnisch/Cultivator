#pragma once

#include "App/AppPlatformBase.hh"

namespace ct::windows
{
	class AppPlatform final : public AppPlatformBase
	{
	public:
		AppPlatform();

		void pollEvents() final override;
	};
}
