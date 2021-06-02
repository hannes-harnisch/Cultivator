#pragma once

#include "PCH.hh"

#include "App/AppContextBase.hh"
#include "Utils/Singleton.hh"

namespace ct::windows
{
	class AppContext final : public AppContextBase, public Singleton<AppContext>
	{
	public:
		static constexpr auto WindowClassName = TEXT(CT_APP_NAME);

		static HINSTANCE nativeInstanceHandle()
		{
			return SingletonInstance->AppHandle;
		}

		AppContext();

		void pollEvents() override;

	private:
		HINSTANCE AppHandle;
	};
}
