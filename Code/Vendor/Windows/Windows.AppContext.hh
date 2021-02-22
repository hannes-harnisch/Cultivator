#pragma once

#include "PCH.hh"
#include "App/AppContextBase.hh"

namespace ct::windows
{
	class AppContext final : public AppContextBase
	{
	public:
		static constexpr auto WindowClassName {TEXT(CT_APP_NAME)};

		inline static HINSTANCE nativeInstanceHandle()
		{
			return Singleton->AppHandle;
		}

		AppContext();

		void pollEvents() override;

	private:
		static inline AppContext* Singleton;

		HINSTANCE AppHandle;
	};
}
