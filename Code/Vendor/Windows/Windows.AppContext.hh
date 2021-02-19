#pragma once

#include "App/AppContextBase.hh"

namespace ct::windows
{
	class AppContext final : public AppContextBase
	{
	public:
		AppContext();

		void pollEvents() final override;
	};
}
