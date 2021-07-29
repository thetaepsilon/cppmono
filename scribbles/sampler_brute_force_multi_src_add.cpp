#include <cstddef>

template <typename T>
struct sum {
    public:
        using elem_t = const T*;
        using src_t = const elem_t*;

        static inline T run(src_t src, size_t length) {
            T result = 0;

            for (size_t i = 0; i < length; i++) {
                elem_t ptr = src[i];
                T addend = *ptr;
                result += addend;
            }

            return result;
        }
};

using test_t = float;
template test_t sum<test_t>::run(sum<test_t>::src_t, size_t);
