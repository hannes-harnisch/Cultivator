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

	impl_->module_handle = ::LoadLibraryW(w_path.c_str());
	if (impl_->module_handle == nullptr) {
		error = windows::last_error_to_error_condition(::GetLastError());
		return;
	}
}

SharedLibrary::~SharedLibrary() {
	if (impl_->module_handle != nullptr) {
		BOOL success = ::FreeLibrary(impl_->module_handle);
		require(success, "failed to unload shared library");
	}
}

SharedLibrary::SharedLibrary(SharedLibrary&& other) noexcept :
	impl_(std::exchange(other.impl_, Impl())) {
}

void* SharedLibrary::load_address(const char* name) {
	return reinterpret_cast<void*>(::GetProcAddress(impl_->module_handle, name));
}

} // namespace cltv
