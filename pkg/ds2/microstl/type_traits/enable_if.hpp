#pragma once

namespace ds2 { namespace microstl { namespace type_traits {

template<bool, typename T = void>
struct enable_if {};

template<typename T>
struct enable_if<true, T> {
	using type = T;
};

// C++14 feature switching...
#if __cplusplus >= 201402L

template <bool B, typename T = void>
using enable_if_t = typename enable_if<B, T>::type;

#endif


}}}

