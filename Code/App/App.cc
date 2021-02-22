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
		Singleton->ShouldTick = false;
	}

	App::App() : MainWindow {CT_APP_NAME, {600, 600}, 400, 400}
	{
		ctEnsure(!Singleton, "App can only be instantiated once.");
		Singleton = this;

		MainWindow.show();
	}

	int App::start()
	{
		while(ShouldTick)
			tick();

		return EXIT_SUCCESS;
	}

	void App::tick()
	{
		AppContext.pollEvents();
	}
}
