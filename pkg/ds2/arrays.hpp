#pragma once

#include <cstddef>
#include <ds2/arrays/range_pointer.hpp>


namespace ds2::array {
// rip indentation



// circular reference forward declare dance...
template <typename T, size_t N>
struct array_ref;

template <typename T, std::size_t MaxBound>
struct array_max_bound;

// template deductable helper function to reference an existing plain array.
template <typename T, size_t N>
constexpr inline array_ref<T, N> ref_plain(T (&)[N]);







// "array pointer" where the array size is known at compile time.
template <typename T, size_t N>
struct array_ref {
private:
	// while the obvious implementation might be this...
	//T (&array)[N];
	// this doesn't allow re-assignment of the object,
	// as you can't change a reference once it exists, only it's referee.
	// however, we can still keep the non-null-ness guarantees via constructors.
	// so we just do a careful dance with pointers to T instead.
	// we could use T(*)[N] too but then we lose constexpr-ness of "slicing" the array,
	// as going from T(&)[3] to T(&)[2] requires a reinterpret_cast,
	// and there's not a workaround that's also constexpr.
	T* data;
	struct __internal {};
	// unsafe constructors, only used when we're handling things and set the ptr ourselves.
	constexpr inline array_ref(__internal&&, T* src): data(src) {}
	//constexpr inline array_ref(T* src): data(src) {}
	// allow differently typed versions of ourselves into our internals.
	// used by slice and co. below.
	// technically we should only allow reference to different lengths,
	// but friends can't be partially specialised it seems.
	template <typename X, size_t L> friend struct array_ref;
public:
	static_assert(N >= 0);
	constexpr inline array_ref(T(&array)[N]): data(&array[0]) {}


	// const-ness conversions - array_ref<T> to array_ref<const T>
	constexpr inline operator array_ref<const T, N>() const {
		using target = array_ref<const T, N>;
		using tag = typename target::__internal;
		return target(tag{}, data);
	}

	// unsafe pointer access.
	// UB occurs if ret[n] is accessed where (n < 0) or (n >= N).
	// in particular this means if N is zero, UB occurs if the pointer is deref'd at all.
	// this should ideally only be used to construct other safe primitives with,
	// or for interop with existing APIs.
	constexpr inline T* unsafe() const { return data; }

	template <size_t L>
	constexpr inline array_ref<T, L> slice() const {
		static_assert(L <= N, "array_ref cannot grow out of bounds!");
		using target = array_ref<T, L>;
		using tag = typename target::__internal;
		array_ref<T, L> ret(tag{}, data);
		return ret;
	}
};
// partial specialisation: in the zero-based case we don't need to hold anything,
// and we can also correct an issue with the constructor (see null() note below).
template <typename T>
struct array_ref<T, 0> {
private:
	template <typename X, size_t L> friend struct array_ref;
	// various bits to make this special case behave like the general one.
	struct __internal {};
	constexpr inline array_ref(__internal&&, T*){}
public:
	// as we have no other way to make such an array without barf
	// (curse you, GNU extensions), provide a specialised static constructor.
	// otherwise in the zero-based case the (&)[N] constructor above causes issues;
	// the mere existance of such a signature (namely T(&)[0])
	// causes gcc and clang to screw up their noses.
	static constexpr inline auto null() {
		return array_ref<T, 0>(__internal{}, nullptr);
	}

	// remember what I said about UB for zero-sized case? ;)
	constexpr inline T* unsafe() { return nullptr; }

	// trivial const-ness cast if needed.
	constexpr inline operator array_ref<const T, 0>() const {
		return array_ref<const T, 0>::null();
	}

	// slice case - we can't get any bigger so...
	template <size_t L>
	constexpr inline array_ref<T, 0> slice() const {
		static_assert(L == 0, "array_ref cannot grow out of bounds!");
		return null();
	}
};



template <typename T, size_t N>
constexpr inline array_ref<T, N> ref_plain(T (&l)[N]) {
	return array_ref<T, N>(l); 
}

// It appears that we can't make an operator "" work for us here.
// C++17 forbade operator"" from having any template arguments,
// and C++20 only allows specific forms of it;
// not to mention compiler support for the latter is lacking.
// None of those lets us get at the length of a string literal in a constexpr way
// (not even consteval can save us here), so we're stuck with this method.
// there's technically nothing preventing this being used on something non-string like,
// partly because we can't anticipate any and all future string types here.

template <typename T, size_t N>
constexpr inline array_ref<const T, N-1> ref_str_lit(const T (&l)[N]) {
	// TIL dependent template lookups are a thing...
	return (ref_plain(l)).template slice<N-1>();
}








// Fixed size array storage structure.
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







// end namespace
}

