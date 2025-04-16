#include "Window.hpp"

namespace cltv {

void Window::on_close() {
	close();
	app_->quit();
}

} // namespace cltv
