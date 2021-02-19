#pragma once

#include "App/AppContext.hh"
#include "App/Window.hh"
#include "Vendor/Vulkan/Vulkan.GraphicsContext.hh"

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

	private:
		static inline App* Singleton;

		AppContext AppContext;
		vulkan::GraphicsContext GraphicsContext;
		Window MainWindow;
		bool ShouldTick = true;

		App();

		int start();
		void tick();
	};
}
