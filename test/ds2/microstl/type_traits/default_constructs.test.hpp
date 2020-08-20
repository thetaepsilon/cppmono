#include <ds2/microstl/type_traits/default_constructs.hpp>

using namespace ds2::microstl::type_traits;

struct A {};
struct B {
	constexpr B(const A& a) {}
};
struct C {
	C() = delete;
};

static_assert(default_constructs<A>);
static_assert(not default_constructs<B>);
static_assert(not default_constructs<C>);

