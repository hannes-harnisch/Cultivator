#pragma once

#include "app/Application.hpp"
#include "util/ImplStorage.hpp"
#include "util/RectSize.hpp"

namespace cltv {

class Window {
public:
	Window(Application* app, std::string_view title, RectSize size, int32_t x, int32_t y);

	~Window();
	Window(Window&&) = delete;

	void on_close();

	void show();
	void close();

	RectSize get_size() const;

#if CLTV_SYSTEM_WINDOWS
	HINSTANCE get_instance_handle() const;
	HWND get_hwnd() const;
#endif

private:
	struct Impl;
	ImplStorage<Impl> impl_;
	Application* app_;
};

} // namespace cltv
