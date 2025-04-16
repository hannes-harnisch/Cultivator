#pragma once

#if CLTV_SYSTEM_WINDOWS
	#include "util/windows/WindowsDef.hpp"
#endif

#include "util/ImplStorage.hpp"

namespace cltv {

class Application {
public:
	explicit Application(std::string_view app_name);

	~Application();
	Application(Application&&) = delete;

	virtual void quit() = 0;

	void poll_events();

	static void show_error_window(std::string_view title, std::string_view message);

#if CLTV_SYSTEM_WINDOWS
	HINSTANCE get_instance_handle() const;
	ATOM get_window_class() const;
#endif

private:
	struct Impl;
	ImplStorage<Impl> impl_;
};

} // namespace cltv
