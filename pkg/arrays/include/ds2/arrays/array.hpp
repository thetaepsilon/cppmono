#pragma once

#include "array_ref.hpp"

namespace ds2::array {

// No I will not drag in std::array, STL is bloat -_-
// Also std::array doesn't allow a *reference* to it's internal array,
// only a non-sized pointer by the usual decay mechanisms,
// which really does defeat the point of what we're doing here.
template <typename T, size_t N>
struct array {
	T data[N];

	constexpr inline operator array_ref<T, N>() {
		return ref_plain(data);
	}

	constexpr inline operator array_ref<const T, N>() const {
		return ref_plain(data);
	}

	// forbid getting a mutable array while const, because data will be const too!
	operator array_ref<T, N>() const = delete;
};



} // end namespace

