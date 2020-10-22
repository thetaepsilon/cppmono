#include <type_traits>
#include <ds2/microstl/type_traits/enable_if.hpp>
// can we modules yet... only having to type one import would be nice.
using namespace ds2::microstl::type_traits;

template <bool b>
constexpr typename enable_if<b>::type accept() {}

// interesting, concepts are backwards from what you'd expect,
// when used to constrain a template parameter.
// the T being constrained always becomes the *first* argument!
template <typename T, typename Parent>
concept DerivedFrom = requires(T t) {
	accept<std::is_base_of_v<Parent, T>>();
};

struct A {};
struct B: public A {};

static_assert(DerivedFrom<B, A>);


template <DerivedFrom<A> T>
constexpr bool foo() { return true; }

static_assert(foo<B>());
static_assert(not DerivedFrom<int, B>);
// note that attempting to call foo<int>() is an instant hard unmet constraint error,
// as we would not be reading the value directly, but negating the value of a function call,
// where said function *always requires* DerivedFrom<T, A> to hold to even be valid.
struct C {};
static_assert(not DerivedFrom<C, B>);
static_assert(not DerivedFrom<A, B>); // <- remember, backwards!


