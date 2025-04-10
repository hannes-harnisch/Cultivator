#include "Window.hpp"

#include "util/Util.hpp"
#include "util/windows/WindowsInclude.hpp"
#include "util/windows/WindowsUtil.hpp"

namespace cltv {

struct Window::Impl {
	HINSTANCE instance_handle;
	HWND hwnd;
};

Window::Window(Application* app, std::string_view title, RectSize size, int32_t x, int32_t y) :
	_app(app) {
	_impl->instance_handle = app->get_instance_handle();

	std::wstring w_title = windows::utf8_to_utf16(title);
	_impl->hwnd = ::CreateWindowExW(0, MAKEINTATOM(app->get_window_class()), w_title.c_str(), WS_OVERLAPPEDWINDOW, x, y,
									size.width, size.height, nullptr, nullptr, _impl->instance_handle, nullptr);
	require(_impl->hwnd != nullptr, "failed to create window");

	::SetLastError(0);
	::SetWindowLongPtrW(_impl->hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
	DWORD err = ::GetLastError();
	require(err == 0, "failed to register window pointer in window data");
}

Window::~Window() {
	BOOL success = ::DestroyWindow(_impl->hwnd);
	require(success, "failed to destroy window");
}

RectSize Window::get_size() const {
	RECT rect;
	BOOL success = ::GetClientRect(_impl->hwnd, &rect);
	require(success, "failed to get window client rect");

	return {
		.width	= rect.right - rect.left,
		.height = rect.bottom - rect.top,
	};
}

void Window::show() {
	::ShowWindow(_impl->hwnd, SW_SHOW); // return value can be ignored here
}

void Window::close() {
	BOOL success = ::CloseWindow(_impl->hwnd);
	require(success, "failed to close window");
}

HINSTANCE Window::get_instance_handle() const {
	return _impl->instance_handle;
}

HWND Window::get_hwnd() const {
	return _impl->hwnd;
}

} // namespace cltv
