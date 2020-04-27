#include <ds2/microstl/type_traits/enable_if.hpp>

/*
Values of TheRefined type that are proven to satisfy the property designated by Constraint.
the Refined struct holds a value of TheRefined that cannot be altered directly
(otherwise it'd be possible to invalidate the constraint),
but that can be copied out or inspected const.

Construction of Refined types is currently limited to methods of construction specified by the Constraint type.
These methods must be constexpr and ideally should not perform exception throws for now;
the intent is that Refined will later handle runtime refinement of a type.
In such a case, either the constraint holds, in which case Refined<...> is returned;
or it does not hold, and an enforceable "does not exist" condition needs to be signalled,
for instance by exception throw or an optional type of some kind like std::optional<Refined<...>>.

Note that Refined<> is supposed to signal a reliably checked constraint;
a Refined<T, C> where C does not hold *cannot exist*, so consumers can depend on C without limitation.
In particular, attempts to obtain a non-const reference to the contained value
are considered undefined behaviour in the C(++) language sense;
the operators and code herein are free to assume you can't do that,
so expect your compiler's optimiser to eat your cat if you ignore this warning. ;)
*/
template <typename TheRefined, typename Constraint>
struct Refined {
private:
	// we don't use a const value here as we get into tricky territory with UB,
	// *even if we still give out only const references*.
	// the consumer would potentially require stunts like std::launder everywhere.
	// instead just protect the value in a more typical way using member functions.
	TheRefined x;

	// helper here: becomes an ill-formed expression if type evaluation fails.
	// we use SFINAE techniques below to avoid causing unnecessary explosions,
	// *but* we're also relying on the constraint allowing conversions for safety.
	// now one could technically sabotage the unnamed SFINAE type parameters,
	// so we (unfortunately) must repeat ourselves a little to validate everything.
	template <typename T> constexpr static bool valid_type = true;

	template <typename T>
	static T&& declval();
public:
	constexpr inline const TheRefined& value() { return x; }

	// Allow construction when the constraint explicitly allows for it,
	// *AND* the types automatically decay to something the unconstrained type understands.
	// This is not without some traps though;
	// in particular, certain types of would-be "narrowing" conversions
	// (e.g. long -> short) on the standard (u)int types can yield surprises;
	// always check with a dumb function first.
	// additionally, any preconditions must be triggered in unevaluated contexts,
	// for instance by using enable_if to whack overload resolution;
	// in particular static_assert() *will not fire* in unevalated contexts!
	template <
		typename... Args,
		typename = decltype(Constraint::passive(declval<Args>()...)),
		typename = decltype(TheRefined(declval<Args>()...))
	>
	constexpr inline Refined(Args&&... args):
		x(static_cast<Args&&>(args)...)
	{
		// we don't need to check that TheRefined constructs,
		// as x() above would otherwise be ill-formed.
		// but check that the SFINAE parameter for ::passive() wasn't tampered with!
		static_assert(valid_type<decltype(Constraint::passive(declval<Args>()...))>);
	}
};







struct strvec {
	const char* data;
	unsigned int len;

	template <unsigned int N>
	constexpr strvec(const char (&data)[N]): data(data), len(N) {}
};

// example: an array at most N elements in size.
template <unsigned int N>
struct at_most {
	template <
		unsigned int L,
		typename = ds2::microstl::type_traits::enable_if_t<L <= N>
	>
	constexpr static void passive(const char (&data)[L]) {
		static_assert(L <= N);
	}
};
extern const Refined<strvec, at_most<5>> test1;
constexpr const Refined<strvec, at_most<5>> test1("abc");

// kaboom: no overload present to construct this.
//extern const Refined<strvec, at_most<5>> test2;
//constexpr const Refined<strvec, at_most<5>> test2("abcdef");


