#include "App.hh"

#include "Assert.hh"
#include <cstdlib>

int main()
{
	static ct::App app;
	return app.run();
}

namespace ct
{
	App::App()
	{
		ctEnsure(!Instance, "App can only be instantiated once.");
		Instance = this;

		initializePlatform();
		MainWindow = Window(CT_APP_NAME, 500, 500, 0, 0);
	}

	int App::run()
	{
		MainWindow.show();

		while(ShouldRun)
			pollEvents();

		return EXIT_SUCCESS;
	}
}
