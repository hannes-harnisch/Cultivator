#include "PCH.hh"
#include "App.hh"

#include "Assert.hh"

int main()
{
	static ct::App app;
	return app.start();
}

namespace ct
{
	App::App() : MainWindow {CT_APP_NAME, {500, 500}, 0, 0}
	{
		ctEnsure(!Singleton, "App can only be instantiated once.");
		Singleton = this;

		MainWindow.show();
	}

	void App::quit()
	{
		ShouldTick = false;
	}

	int App::start()
	{
		while(ShouldTick)
			tick();

		return EXIT_SUCCESS;
	}

	void App::tick()
	{
		AppPlatform.pollEvents();
	}
}
