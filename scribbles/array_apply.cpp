/*
Constexpr array operations like concatenation (!),
using a helper container around an array that calls a provided functor in it's constructor.
*/

template <typename T, unsigned int L, typename F>
struct array_apply {
	T data[L];

	// note we still have to specify data() even if we touch all array members,
	// to satisfy the definition of a constexpr function.
	// (untouched elements are default or zero initialised.)
	constexpr inline array_apply(F f): data() {
		f(data);
	}
};

constexpr const int pattern[4] = {
	0, 1, 2, 3
};

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

extern const array_apply<int, 64, repeat<int, 4, 16>> test;
constexpr const array_apply<int, 64, repeat<int, 4, 16>> test(mk_repeat<16, int, 4>(pattern));

