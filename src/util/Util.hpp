#pragma once

namespace cltv {

inline void require(bool condition, const char* msg) {
	if (!condition) {
		throw std::runtime_error(msg);
	}
}

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

} // namespace cltv
