#pragma once

// C++14 due to use of templated variables.
// still no nested namespace specifiers...

namespace ds2 { namespace microstl { namespace type_traits {

template <typename A, typename B>
constexpr bool is_same = false;

template <typename T>
constexpr bool is_same<T, T> = true;

}}}

