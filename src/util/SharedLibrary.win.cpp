#include "SharedLibrary.hpp"

#include "util/Util.hpp"
#include "util/windows/WindowsInclude.hpp"
#include "util/windows/WindowsUtil.hpp"

namespace cltv {

struct SharedLibrary::Impl {
	HMODULE module_handle;
};

SharedLibrary::SharedLibrary(std::string_view path, std::error_condition& error) {
	std::wstring w_path = windows::utf8_to_utf16(path);

	_impl->module_handle = ::LoadLibraryW(w_path.c_str());
	if (_impl->module_handle == nullptr) {
		error = windows::last_error_to_error_condition(::GetLastError());
		return;
	}
}

SharedLibrary::~SharedLibrary() {
	if (_impl->module_handle != nullptr) {
		BOOL success = ::FreeLibrary(_impl->module_handle);
		require(success, "failed to unload shared library");
	}
}

SharedLibrary::SharedLibrary(SharedLibrary&& other) noexcept :
	_impl(std::exchange(other._impl, Impl())) {
}

void* SharedLibrary::load_address(const char* name) {
	return reinterpret_cast<void*>(::GetProcAddress(_impl->module_handle, name));
}

} // namespace cltv
