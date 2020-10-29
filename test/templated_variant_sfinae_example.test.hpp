#include <ds2/microstl/type_traits/enable_if.hpp>
#include <testlib/sfinae_reader.hpp>

using namespace ds2::microstl::type_traits;

template <unsigned N, typename = enable_if_t<(N > 0)>>
struct under_test {
	// backup to catch the case of the above enable_if condition not causing a deduction fail;
	// bring the house down instead of allowing the sfinae gadget below to return true incorrectly.
	static_assert(N > 0, "WTF condition: SFINAE imploded");
};

#define sfinae_args N
sfinae_reader(typechecks, under_test, unsigned N)

static_assert(typechecks<1> == true, "1 > 0");
static_assert(typechecks<2> == true, "2 > 0");
static_assert(typechecks<0> == false, "0 not greater than 0, should trigger SFINAE fallback");

