#include "Window.hpp"

namespace cltv {

void Window::on_close() {
	close();
	_app->quit();
}

} // namespace cltv
