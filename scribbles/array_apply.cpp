/*
Constexpr array operations like concatenation (!),
using a helper container around an array that calls a provided functor in it's constructor.
*/



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



template <typename T, unsigned int L>
struct array_apply {
	T data[L];

	template <typename Functor>
	constexpr inline array_apply(Functor&& f): data() {
		f(data);
	}
};

extern    const array_apply<int, 64> t1;
constexpr const array_apply<int, 64> t1(mk_repeat<16, int, 4>(pattern));



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


constexpr const int src1[2] = {0, 1};
constexpr const int src2[4] = {0, 1, 2, 3};
extern    const array_apply<int, 6> t2;
constexpr const array_apply<int, 6> t2(concat(src1, src2));









