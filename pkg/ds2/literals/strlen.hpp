#pragma once
#include <cstddef>

namespace ds2::literals::strlen {

	constexpr inline size_t operator "" _length(const char*, size_t length) {
    		return length;
	}

}

