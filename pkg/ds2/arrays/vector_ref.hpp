#pragma once
#include <cstddef>
#include "array_ref.hpp"
#include "range_pointer.hpp"

/*
In reference to the STL's std::vector,
this is a non-owning reference to a runtime-sized array of objects,
essentially a pointer to the array and it's length.
*/

namespace ds2::array {
	template <typename T>
	struct vector_ref {
	private:
		T* data;
		std::size_t len;
		constexpr inline vector_ref(T* data, std::size_t length):
			data(data), len(length) {}
	public:
		// named "unsafe" to hint to the caller they should be very sure before using this.
		// also makes it stand out in code more.
		static constexpr inline vector_ref<T> from_unsafe(T* data, std::size_t length) {
			return vector_ref<T>(data, length);
		}

		// support "type erasing" a compile-time sized array into a runtime sized one.
		template <std::size_t N>
		constexpr inline vector_ref(T (&array)[N]):
			data(&array[0]), len(N) {}
		template <std::size_t N>
		constexpr inline vector_ref(const array_ref<T, N>& ref):
			data(ref.unsafe()), len(N) {}

		// range-for protocol
		// (should work with C++14, begin and end are same type)
		constexpr inline auto begin() { return range_pointer<T>(data); }
		constexpr inline auto end() { return range_pointer<T>(&data[len]); }

		// allow unsafe random access but make it stand out.
		constexpr inline T* unsafe() { return data; }
	};
}

