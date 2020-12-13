#pragma once
#include <cstddef>

namespace ds2 {
namespace arrays {

	template <typename T, size_t N>
	constexpr size_t array_size(T (&)[N]) {
		return N;
	}

}
}

