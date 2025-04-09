#include "Application.hpp"

#include "app/Window.hpp"
#include "util/Util.hpp"
#include "util/windows/WindowsInclude.hpp"
#include "util/windows/WindowsUtil.hpp"

namespace cltv {

static LRESULT CALLBACK window_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) {
	Window* window = reinterpret_cast<Window*>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA));
	if (window != nullptr) {
		switch (message) {
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;

		case WM_CLOSE:
			window->on_close();
			return 0;
		}
	}
	return ::DefWindowProcW(hwnd, message, w_param, l_param);
}

struct Application::Impl {
	HINSTANCE instance;
	ATOM window_class;
};

Application::Application(std::string_view app_name) {
	_impl->instance = ::GetModuleHandleW(nullptr);
	require(_impl->instance != nullptr, "failed to get module handle");

	std::wstring w_app_name = windows::utf8_to_utf16(app_name);

	WNDCLASSW wnd_class {
		.style		   = CS_DBLCLKS,
		.lpfnWndProc   = window_proc,
		.cbWndExtra	   = sizeof(Window*),
		.hInstance	   = _impl->instance,
		.lpszClassName = w_app_name.c_str(),
	};
	_impl->window_class = ::RegisterClassW(&wnd_class);
	require(_impl->instance != 0, "failed to register window class");
}

Application::~Application() {
	BOOL success = ::UnregisterClassW(MAKEINTATOM(_impl->window_class), _impl->instance);
	require(success, "failed to unregister window class");
}

void Application::poll_events() {
	BOOL success = ::WaitMessage();
	require(success, "failed to wait for messages");

	MSG msg;
	while (::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) {
			quit();
			break;
		}
		::TranslateMessage(&msg);
		::DispatchMessageW(&msg);
	}
}

void Application::show_error_window(std::string_view title, std::string_view message) {
	auto w_title   = windows::utf8_to_utf16(title);
	auto w_message = windows::utf8_to_utf16(message);
	::MessageBoxW(nullptr, w_message.c_str(), w_title.c_str(), MB_OK | MB_ICONERROR);
}

HINSTANCE Application::get_instance_handle() const {
	return _impl->instance;
}

ATOM Application::get_window_class() const {
	return _impl->window_class;
}

} // namespace cltv
