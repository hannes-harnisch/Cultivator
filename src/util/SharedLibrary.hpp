#pragma once

#include "util/ImplStorage.hpp"

namespace cltv {

class SharedLibrary {
public:
	SharedLibrary(std::string_view path, std::error_condition& error);

	~SharedLibrary();
	SharedLibrary(SharedLibrary&&) noexcept;

	void* load_address(const char* name);

	template<typename T>
	T load_symbol(const char* name) {
		return reinterpret_cast<T>(load_address(name));
	}

private:
	struct Impl;
	ImplStorage<Impl> _impl;
};

} // namespace cltv
