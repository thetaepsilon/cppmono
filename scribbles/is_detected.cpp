// once again, who needs the entire STL header...
#include <ds2/microstl/type_traits/enable_if.hpp>




// from the uncommonly used comma operator, that "discards" it's first "value".
// we're using it below to trigger SFINAE in return type context.
template <typename Phantom, typename T>
struct comma {
	using phantom = Phantom;
	T value;
	constexpr comma(T v): value(v) {}
};
// whereas here we pass through a single type.
// not having the "phantom" type here is intentional.
template <typename T>
struct id {
	T value;
	constexpr id(T v): value(v) {}
};

/*
template <template <typename...> class Op, typename...>
constexpr id<bool> is_detected_(...) { return false; }
*/

template <template <typename...> class Op, typename... X>
constexpr comma<Op<X...>, bool> is_detected_(int) { return true; }

template <template <typename...> class Op, typename... X>
constexpr bool is_detected = is_detected_<Op, X...>(0).value;


// an annoying hitch with parameter pack expansions for alias templates
// (see https://stackoverflow.com/questions/30707011/ )
// means this ends up being more verbose than needed.
// we have to peel away arguments manually otherwise GCC at least will barf.
// clang meanwhile would happily just accept the following form:
/*
template <typename X, typename...>
using default_construct = decltype(X());
*/
// I for one don't want to be sensitive to implementation divergence,
// but this alas makes the usage of is_detected (heck, even the std:: one)
// convoluted enough as to simply make it faster to write SFINAE boilerplate per-case.

template <typename X, typename...>
struct peel {
	using type = X;
};

template <typename... Args>
using default_construct = decltype(typename peel<Args...>::type());

struct S {};
static_assert(is_detected<default_construct, S>);
/*
struct N {
	N() = delete;
};
static_assert(not is_detected<default_construct, N>);
*/




/*
struct type_level {
	type_level() = delete;
	type_level(type_level&) = delete;
	type_level& operator=(type_level) = delete;
};
*/

