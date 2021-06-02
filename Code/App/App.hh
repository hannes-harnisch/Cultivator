#pragma once

#include "App/AppContext.hh"
#include "App/Window.hh"
#include "Graphics/CellularAutomatonRenderer.hh"
#include "Vendor/Vulkan/Vulkan.GPUContext.hh"

int main();

namespace ct
{
	class App final : public Singleton<App>
	{
		friend int ::main();

	public:
		static void quit();

		App(App const&) = delete;
		App& operator=(App const&) = delete;

	private:
		AppContext AppContext;
		vulkan::GPUContext GPUContext;
		Window MainWindow;
		CellularAutomatonRenderer Renderer;
		bool ShouldTick = true;

		App();

		int start();
		void tick();
	};
}
