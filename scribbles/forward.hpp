// probably gonna move this later.
template <typename T>
constexpr T&& forward(T& t) {
	return static_cast<T&&>(t);
}

