#include <ds2/microstl/type_traits/enable_if.hpp>
#include <testlib/sfinae_reader.hpp>

using namespace ds2::microstl::type_traits;

template <unsigned N, typename = enable_if_t<(N > 0)>>
struct under_test {
	static_assert(N > 0);
};

#define sfinae_args N
sfinae_reader(typechecks, under_test, unsigned N)

static_assert(typechecks<1> == true);
static_assert(typechecks<2> == true);
static_assert(typechecks<0> == false);

