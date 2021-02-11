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

		void quit();

		App(const App&) = delete;
		App(App&&)		= delete;

		App& operator=(const App&) = delete;
		App& operator=(App&&) = delete;

	private:
		static inline App* Instance;

		bool ShouldTick = true;
		Window MainWindow;

		App();

		int start();
		void tick();

		void initializePlatform();
		void pollEvents();
	};
}
