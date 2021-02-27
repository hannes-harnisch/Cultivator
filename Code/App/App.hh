#pragma once

#include "App/AppContext.hh"
#include "App/Window.hh"
#include "Graphics/CellularAutomatonRenderer.hh"
#include "Vendor/Vulkan/Vulkan.GraphicsContext.hh"

int main();

namespace ct
{
	class App final : public Singleton<App>
	{
		friend int ::main();

	public:
		static void quit();

		App(const App&) = delete;
		App& operator=(const App&) = delete;

	private:
		AppContext AppContext;
		vulkan::GraphicsContext GraphicsContext;
		Window MainWindow;
		CellularAutomatonRenderer Renderer;
		bool ShouldTick = true;

		App();

		int start();
		void tick();
	};
}
