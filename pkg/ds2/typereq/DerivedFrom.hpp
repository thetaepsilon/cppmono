#pragma once
#include <type_traits>
#include <ds2/microstl/type_traits/enable_if.hpp>
// can we modules yet... only having to type one import would be nice.

namespace ds2::typereq::DerivedFrom {

	namespace detail {
		using namespace ds2::microstl::type_traits;

		// XXX: I suspect this will become used in more concepts yet.
		template <bool b>
		constexpr typename enable_if<b>::type accept() {}
	}

	// interesting, concepts are backwards from what you'd expect,
	// when used to constrain a template parameter.
	// the T being constrained always becomes the *first* argument!
	template <typename T, typename Parent>
	concept DerivedFrom = requires(T t) {
		detail::accept<std::is_base_of_v<Parent, T>>();
	};

}

