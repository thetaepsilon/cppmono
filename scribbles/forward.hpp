#pragma once

// This is for all intents and purposes C++11 compatible,
// so we'll have to put up with lack of nested namespaces. -_-

namespace ds2 { namespace microstl { namespace utility { namespace forward {

template <typename T>
constexpr T&& forward(T& t) {
	return static_cast<T&&>(t);
}

}}}}

