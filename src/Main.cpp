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
		window_(this, "Cultivator", {1920, 1080}, 100, 100) {
	}

	void run() {
		window_.show();

		std::thread render_thread(&Cultivator::render_loop, this);

		while (should_run_) {
			poll_events();
		}

		render_thread.join();
	}

	void quit() override {
		should_run_ = false;
	}

private:
	Window window_;
	std::atomic_bool should_run_ = true;

	void render_loop() {
		DeviceContext context(window_, EnableDebugLayer);

		RendererParams params {
			.universe_size				 = RectSize {.width = 480, .height = 270},
			.simulation_shader_path		 = "GameOfLife.frag.spv",
			.initial_live_cell_incidence = 10,
			.delay_milliseconds			 = 100,
		};
		AutomatonRenderer renderer(&context, &window_, params);

		while (should_run_) {
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
