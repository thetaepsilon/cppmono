
#include <cstddef>

constexpr inline size_t operator "" _static_length(const char*, size_t length) {
    return length;
}



#define __ds2_utils_macro_paste5_(x, y, z, a, b) x ## y ## z ## a ## b
#define __ds2_utils_macro_paste5(x, y, z, a, b) \
    __ds2_utils_macro_paste5_(x, y, z, a, b)
#define __ds2_utils_macro_paste8_(x, y, z, a, b, c, d, e) \
    x ## y ## z ## a ## b ## c ## d ## e
#define __ds2_utils_macro_paste8(x, y, z, a, b, c, d, e) \
	__ds2_utils_macro_paste8_(x, y, z, a, b, c, d, e)

#define __ds2_unique_name(x, label) \
	__ds2_utils_macro_paste8(__, x, , , __anon__line, __LINE__, __, label)

#define __ds2_arrays_buffer_fromlit_inner(tmpname, T, len, s, R, n, f) \
	constexpr T tmpname [ len ] = s;\
	constexpr R n = f(tmpname, s ## _static_length)

#define __ds2_arrays_buffer_fromlit(T, len, s, R, n, f) \
    __ds2_arrays_buffer_fromlit_inner(__ds2_unique_name(x, buffer_storage), T, len, s, R, n, f)


template <typename T>
struct buffer_ref {
	T* data;
	size_t capacity;
	size_t used;
};
template <typename T, size_t N>
constexpr inline auto buffer_from_array(T (&arr)[N], size_t used) {
	return buffer_ref<T> { arr, N, used };
}

__ds2_arrays_buffer_fromlit(static char, 32, "asdf", extern const auto, wat, buffer_from_array);

