#include <ds2/microstl/type_traits/default_constructs.hpp>

using namespace ds2::microstl::type_traits;

struct A {};
struct B {
	constexpr B(const A& a) {}
};
struct C {
	C() = delete;
};

static_assert(default_constructs<A>, "because A has an implicit default constructor");
static_assert(
	not default_constructs<B>,
	"because B has a explicitly specified constructor with arguments");
static_assert(not default_constructs<C>, "because C deletes the default constructor");

