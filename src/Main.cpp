#include "app/Application.hpp"
#include "app/Window.hpp"
#include "graphics/AutomatonRenderer.hpp"

namespace cltv {

#ifdef NDEBUG
const bool EnableDebugLayer = false;
#else
const bool EnableDebugLayer = true;
#endif

class Cultivator final : public Application {
public:
	Cultivator() :
		Application("Cultivator"),
		_window(this, "Cultivator", {1920, 1080}, 100, 100) {
	}

	void run() {
		_window.show();

		std::thread render_thread(&Cultivator::render_loop, this);

		while (_should_run) {
			poll_events();
		}

		render_thread.join();
	}

	void quit() override {
		_should_run = false;
	}

private:
	Window _window;
	std::atomic_bool _should_run = true;

	void render_loop() {
		DeviceContext context(_window, EnableDebugLayer);

		RendererParams params {
			.universe_size				 = RectSize {.width = 480, .height = 270},
			.simulation_shader_path		 = "GameOfLife.frag.spv",
			.initial_live_cell_incidence = 10,
			.delay_milliseconds			 = 500,
		};
		AutomatonRenderer renderer(&context, _window, params);

		while (_should_run) {
			renderer.draw_frame();
		}
	}
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
