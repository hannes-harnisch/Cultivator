#include "Util.hpp"

namespace cltv {

void require(bool condition, std::string_view msg) {
	if (!condition) {
		throw std::runtime_error(std::string(msg));
	}
}

std::optional<std::vector<char>> get_all_file_bytes(const char* path) {
	std::ifstream stream(path, std::ios::binary | std::ios::ate);
	if (!stream.is_open()) {
		return std::nullopt;
	}

	std::streampos size = stream.tellg();
	std::vector<char> bytes(size);

	stream.seekg(0, std::ios::beg);
	stream.read(bytes.data(), size);
	return bytes;
}

} // namespace cltv
