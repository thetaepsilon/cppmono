#pragma once
/*
A restricted pointer struct that only allows incrementing
(I'll probably add decrement later if needed),
and deliberately deletes operator[], but allows deferencing.
This is mainly to be used for range comparisons,
like classic vector begin()/end() iteration or the newer range-for construct.
operator[] is deleted because there is no way to tell array bounds from a pointer alone;
if one really REALLY needs to do random access into a raw array pointer,
it is intended that you call e.g. unsafe or unsafe_ptr on the other array package structs.

One could technically still get a pointer out of this vs using unsafe_ptr say,
by re-taking the address of the reference from operator*,
but that still satisfies the purpose of calling unsafe_ptr() -
it's odd-looking and stands out in code.
*/

namespace ds2::array {
	template <typename T>
	struct range_pointer {
	private:
		T* data;
	public:
		constexpr inline range_pointer(T* t): data(t) {}
		constexpr inline T& operator*() const { return *data; }
		constexpr inline range_pointer& operator++() {
			data++;
			return *this;
		}
		// C++20 isn't really here yet, so can't use the auto-generation from operator<=>().
		// I only found clang to support that as of 20200225,
		// and I'd rather not inadvertently rely on other C++20 features by enabling that version.
		constexpr inline bool operator!=(const range_pointer& other) const {
			return this->data != other.data;
		}
	};
}

