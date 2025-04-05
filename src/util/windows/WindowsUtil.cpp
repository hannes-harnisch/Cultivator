#include "WindowsUtil.hpp"

#include "util/windows/WindowsInclude.hpp"

namespace cltv::windows {

std::wstring utf8_to_utf16(std::string_view utf8_str) {
	const size_t size = utf8_str.size();
	const int length  = static_cast<int>(size);

	// MultiByteToWideChar does not accept empty strings
	if (length == 0) {
		return {};
	}

	std::wstring utf16_str(size, L'\0');

	const int written = ::MultiByteToWideChar(CP_UTF8, 0, utf8_str.data(), length, utf16_str.data(), length);
	utf16_str.resize(written);

	return utf16_str;
}

std::string utf16_to_utf8(std::wstring_view utf16_str) {
	const int length = static_cast<int>(utf16_str.size());

	// WideCharToMultiByte does not accept empty strings
	if (length == 0) {
		return {};
	}

	// a UTF-16 sequence can convert to a UTF-8 sequence of at most triple the number of code units
	const int cap = 3 * length;
	std::string utf8_str(static_cast<size_t>(cap), '\0');

	const int written = ::WideCharToMultiByte(CP_UTF8, 0, utf16_str.data(), length, utf8_str.data(), cap, nullptr, nullptr);
	utf8_str.resize(written);

	return utf8_str;
}

std::error_condition last_error_to_error_condition(DWORD last_error) {
	return std::system_category().default_error_condition(last_error);
}

} // namespace cltv::windows
