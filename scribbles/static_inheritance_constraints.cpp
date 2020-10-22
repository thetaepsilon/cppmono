#include <ds2/typereq/DerivedFrom.hpp>
using namespace ds2::typereq::DerivedFrom;

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


