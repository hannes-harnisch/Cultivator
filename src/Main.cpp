#include "app/Application.hpp"
#include "app/Window.hpp"
#include "graphics/AutomatonRenderer.hpp"

namespace cltv {

class Cultivator final : public Application {
public:
	Cultivator() :
		Application("Cultivator") {
	}

	void run() {
		Window window(this, "Cultivator", {1366, 768}, 100, 100);
		window.show();

		AutomatonRenderer renderer(window, {480, 720});

		while (_should_run) {
			renderer.draw_frame();
			poll_events();
		}
	}

	void quit() override {
		_should_run = false;
	}

private:
	bool _should_run = true;
};

void terminate_handler() {
	try {
		std::rethrow_exception(std::current_exception());
	} catch (const std::exception& e) {
		Application::show_error_window("Error", e.what());
	}
}

} // namespace cltv

int main() try {
	std::set_terminate(cltv::terminate_handler);

	cltv::Cultivator cultivator;
	cultivator.run();
	return EXIT_SUCCESS;

} catch (const std::exception& e) {
	cltv::Application::show_error_window("Error", e.what());
	return EXIT_FAILURE;
}
