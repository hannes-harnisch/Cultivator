#pragma once

#include "Platform/Vulkan/Vulkan.GraphicsPlatform.hh"
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
			return *Singleton;
		}

		void quit();

		App(const App&) = delete;
		App& operator=(const App&) = delete;

		App(App&&)	 = delete;
		App& operator=(App&&) = delete;

	private:
		static inline App* Singleton;

		vulkan::GraphicsPlatform GraphicsPlatform;
		Window MainWindow;
		bool ShouldTick = true;

		App();

		int start();
		void tick();

		void initializePlatform();
		void pollEvents();
	};
}
