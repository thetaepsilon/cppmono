
#include <cstddef>
#include <ds2/ppmacros/unique_name.h>

constexpr inline size_t operator "" _length(const char*, size_t length) {
    return length;
}



#define __ds2_arrays_buffer_fromlit_named(tmpname, T, len, s, R, n, f) \
	constexpr T tmpname [ len ] = s;\
	constexpr R n = f(tmpname, s ## _length)

#define __ds2_arrays_buffer_fromlit(T, len, s, R, n, f) \
    __ds2_arrays_buffer_fromlit_named(__ds2__ppmacros__unique_name(x, buffer_storage), T, len, s, R, n, f)


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

