#pragma once

#include "util/windows/WindowsDef.hpp"

namespace cltv::windows {

std::wstring utf8_to_utf16(std::string_view utf8_str);

std::string utf16_to_utf8(std::wstring_view utf16_str);

std::error_condition last_error_to_error_condition(DWORD last_error);

} // namespace cltv::windows
