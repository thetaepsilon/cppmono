/*
Constexpr array operations like concatenation (!),
using a helper container around an array that calls a provided functor in it's constructor.
*/
#include <cstddef>
#include <cstdint>
#include <ds2/array.hpp>



template <typename T, unsigned int Ls, unsigned int Rep>
struct repeat {
	const T (&source)[Ls];

	constexpr inline repeat(const T (&src)[Ls]): source(src) {}

	constexpr inline void operator()(T* target) {
		for (unsigned int r = 0; r < Rep; r++) {
			for (unsigned int i = 0; i < Ls; i++) {
				target[i] = source[i];
			}
			target += Ls;
		}
	}
};

template <unsigned int Rep, typename T, unsigned int Ls>
constexpr inline repeat<T, Ls, Rep> mk_repeat(const T (&src)[Ls]) {
	return repeat<T, Ls, Rep>(src);
}



template <typename T, unsigned int L>
struct array_apply {
	T data[L];

	template <typename Functor>
	constexpr inline array_apply(Functor&& f): data() {
		f(data);
	}
};

/*
constexpr const int pattern[4] = {
	0, 1, 2, 3
};

extern    const array_apply<int, 64> t1;
constexpr const array_apply<int, 64> t1(mk_repeat<16, int, 4>(pattern));
*/



// woo, real lambda time
template <typename T, unsigned int L1, unsigned int L2>
constexpr auto concat(const T (&src1)[L1], const T (&src2)[L2]) {
	return [&](T (&target)[L1+L2]) constexpr {
		T* t = target;
		unsigned int i = 0;
		// constexpr complains even though we could set it here...
		for (; i < L1; i++) {
			*t++ = src1[i];
		}
		for (i = 0; i < L2; i++) {
			*t++ = src2[i];
		}
	};
}



/*
constexpr const int src1[2] = {0, 1};
constexpr const int src2[4] = {0, 1, 2, 3};
//extern    const array_apply<int, 6> t2;
extern constexpr const array_apply<int, 6> t2(concat(src1, src2));
*/















// now it gets a bit more complicated, bringing in sized array types ono...
// ref = some array isn't quite right. we're on about an overwrite operation.
// though probably just more worthwhile to direct-init the array.
// real C++ programmers can deal with knowing where their data is going.


typedef std::uint8_t u8;
using namespace ds2::array;

constexpr auto mk_write_pixel(u8 r, u8 g, u8 b) {
	return [=](size_t idx, array_ref<u8, 3> pixel) constexpr {
		// TODO: allow a different ordering here?
		pixel.at<0>() = idx & r;
		pixel.at<1>() = idx & g;
		pixel.at<2>() = idx & b;
	};
}

// more strictly typed form of the main array_apply utility using array_ref
template <typename T, unsigned int L>
struct baked_array {	// mmm, baked
	T data[L];

	template <typename Functor>
	constexpr inline baked_array(Functor&& f): data() {
		auto ref = ref_plain(data);
		f(ref);
	}
};

// XXX: this could probably go in the arrays namespace itself eventually.

template <typename T, size_t Length, size_t Sub, typename Functor>
constexpr auto pixel_loop(Functor&& inner) {
	return [&](array_ref<T, Length*Sub> ref) {
		ref.template foreach_segmented<Sub>(inner);
	};
}



baked_array<u8, 768> test3( pixel_loop<u8, 256, 3>(mk_write_pixel(0, 255, 0)) );

