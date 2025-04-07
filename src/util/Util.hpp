#pragma once

namespace cltv {

template<typename R, typename T, typename Projection = std::identity>
bool contains(const R& range, const T& value, Projection projection = {}) {
	auto begin = std::begin(range);
	auto end   = std::end(range);
	return std::ranges::find(begin, end, value, projection) != end;
}

template<typename T>
bool has_flags(T mask, T flags) {
	return (mask & flags) == flags;
}

void require(bool condition, std::string_view msg);

std::optional<std::vector<char>> get_all_file_bytes(const char* path);

} // namespace cltv
