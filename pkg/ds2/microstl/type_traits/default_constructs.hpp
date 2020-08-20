#pragma once

namespace ds2 { namespace microstl { namespace type_traits {



template <typename>
constexpr bool default_constructs_(...) { return false; }

template <typename X, typename = decltype(X())>
constexpr bool default_constructs_(int) { return true; }

template <typename X>
constexpr bool default_constructs = default_constructs_<X>(0);



}}}
