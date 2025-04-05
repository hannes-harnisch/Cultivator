#pragma once

namespace cltv {

/**
 * Helper for hiding implementation details without dynamic allocation.
 */
template<typename T, size_t Size = 16, size_t Align = alignof(void*)>
class ImplStorage {
public:
	ImplStorage() { // NOLINT(*-pro-type-member-init)
		static_assert_size_align();
		new (_storage) T();
	}

	explicit(false) ImplStorage(T&& t) { // NOLINT(*-pro-type-member-init)
		static_assert_size_align();
		new (_storage) T(std::move(t));
	}

	ImplStorage(ImplStorage&& other) noexcept { // NOLINT(*-pro-type-member-init)
		static_assert_size_align();
		new (_storage) T(std::move(*other));
	}

	~ImplStorage() {
		(*this)->~T();
	}

	ImplStorage& operator=(ImplStorage&& other) noexcept {
		**this = std::move(*other);
		return *this;
	}

	T& operator*() & noexcept {
		return *std::launder(reinterpret_cast<T*>(_storage));
	}

	const T& operator*() const& noexcept {
		return *std::launder(reinterpret_cast<const T*>(_storage));
	}

	T&& operator*() && noexcept {
		return std::move(*std::launder(reinterpret_cast<T*>(_storage)));
	}

	T* operator->() noexcept {
		return std::launder(reinterpret_cast<T*>(_storage));
	}

	const T* operator->() const noexcept {
		return std::launder(reinterpret_cast<const T*>(_storage));
	}

private:
	alignas(Align) unsigned char _storage[Size];

	static void static_assert_size_align() {
		static_assert(Size >= sizeof(T) && Align >= alignof(T), "Specified size and alignment must be greater or equal to the "
																"size and alignment of T.");
	}
};

} // namespace cltv
