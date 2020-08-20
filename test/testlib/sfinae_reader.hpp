// I am deeply against this but it's still way easier to type than the boilerplate it expands to.
#define sfinae_reader(name, templ, ...) \
	template < __VA_ARGS__ > constexpr bool name ## _ (...) { return false; } \
	template < __VA_ARGS__ , typename = templ < sfinae_args > > constexpr bool name ## _ (int) { return true; } \
	template < __VA_ARGS__ > constexpr bool name = name ## _ < sfinae_args >(0);

