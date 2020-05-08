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
	// forward decls, see after vector_ref below
	template <typename T, std::size_t MaxBound>
	struct array_max_bound;

	template <typename T>
	struct vector_ref {
	private:
		T* data;
		std::size_t len;
		constexpr inline vector_ref(T* data, std::size_t length):
			data(data), len(length) {}
		// only allow certain friends...
		// it should not be possible to subvert the
		// length-pseudo-constness part from an arbitary subclass.
		// they are defined below to prevent another header masquerading.
		template <typename E, std::size_t N>
		friend struct array_max_bound;
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





	// subclass of the above with known max bound,
	// useful for safe array copies and/or worst-case size usage.
	template <typename T, std::size_t MaxBound>
	struct array_max_bound: public vector_ref<T> {
	private:
		using super = vector_ref<T>;
	public:
		template <std::size_t N>
		constexpr inline array_max_bound(T (&array)[N]):
			super(&array[0], N)
		{
			static_assert(N <= MaxBound);
		}

		template <std::size_t N>
		constexpr inline array_max_bound(const array_ref<T, N>& ref):
			super(ref.unsafe(), N)
		{
			static_assert(N <= MaxBound);
		}
	};
}

