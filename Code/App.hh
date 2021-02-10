#pragma once

#include "Window.hh"

int main();

namespace ct
{
	class App
	{
		friend int ::main();

	public:
		inline static App& get()
		{
			return *Instance;
		}

		inline void exit()
		{
			ShouldRun = false;
		}

		App(const App&) = delete;
		App(App&&)		= delete;
		App& operator=(const App&) = delete;
		App& operator=(App&&) = delete;

	private:
		static inline App* Instance;

		bool ShouldRun = true;
		Window MainWindow;

		App();

		int run();
		void initializePlatform();
		void pollEvents();
	};
}
