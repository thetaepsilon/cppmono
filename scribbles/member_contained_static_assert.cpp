/*
use templated member functions to statically assert a condition on a templated class,
BUT without bringing the entire class down.
this can be useful to only "enable" a member for certain values of the class's type parameters.
try this out using -fsyntax-only.
*/

// normally, one cannot simply do something like the following in all versions of C++,
// even if one does not use foo() on the instantiated class:
/*
template <typename T>
struct thing {
	void foo() {
		static_assert(something_about<T>(), "something about T was wrong");
	}
};
// ...
void bar() {
	// assume wrong_type causes something_about<wrong_type>() to return false
	thing<wrong_type> t1;	// BANG
}
*/
/*
The reason for this is because once we've already instantiated thing,
naturally the static_assert "always exists" because the member function always does on that type.
(Sorry I don't know the language lawyer terms to describe this more accurately.)
*/



// quick re-impl of is_same to aid compilation speed, collapse this if needed
#if 1
template <typename A, typename B>
struct is_same_t {
	constexpr static const bool value = false;
};

template <typename T>
struct is_same_t<T, T> {
	constexpr static const bool value = true;
};

template <typename A, typename B>
inline constexpr static bool is_same() {
	return is_same_t<A, B>::value;
}
#endif





template <typename T>
struct t {
	template <int = 0>
	static void blargh() {
		static_assert(
			is_same<T, char>(),
			"class type parameter must be char to enable this function.");
	}
	static void run();
};

void test() {
	t<char>::blargh();
	t<char>::run();

	// note that this still works, because blargh() is still technically a template,
	// and therefore isn't instantiated here.
	t<int>::run();
	// all these are BANG:
	//t<const char>::blargh();
	//t<int>::blargh();
}
