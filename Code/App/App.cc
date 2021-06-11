#include "PCH.hh"

#include "App.hh"
#include "Utils/Assert.hh"

int main()
{
	static ct::App app;
	return app.start();
}

namespace ct
{
	void App::quit()
	{
		SingletonInstance->shouldTick = false;
	}

	App::App() : window(CT_APP_NAME, {1920, 1080}, 0, 0), renderer({480, 270}, window)
	{
		window.show();
	}

	int App::start()
	{
		while(shouldTick)
			tick();

		return EXIT_SUCCESS;
	}

	void App::tick()
	{
		renderer.draw();
		appContext.pollEvents();
	}
}
