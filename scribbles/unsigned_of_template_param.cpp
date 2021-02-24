struct explode{};

template <typename T>
struct device {
	// verdict: in C++17 at least, this is a hard error _at template define time_. rip
	using type = unsigned T;
};



// just a technique to have the compiler show us what the type ends up being, if any.
typename device<int>::type bang = explode{};

