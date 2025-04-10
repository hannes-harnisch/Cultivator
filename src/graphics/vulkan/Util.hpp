#pragma once

namespace cltv {

[[noreturn]] inline void fail_vk_result(VkResult result, const char* msg) {
	throw std::runtime_error(msg + std::string(", VkResult: ") + std::to_string(result));
}

inline void require_vk_result(VkResult result, const char* msg) {
	if (result != VK_SUCCESS) {
		fail_vk_result(result, msg);
	}
}

uint32_t get_count(const auto& range) {
	return static_cast<uint32_t>(std::size(range));
}

} // namespace cltv
