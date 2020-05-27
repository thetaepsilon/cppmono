#include <ds2/microstl/type_traits/enable_if.hpp>
#include <ds2/microstl/declval.hpp>

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

	// TODO: yeah, I should probably at least have this in a header later,
	// even if I'm not using the STL one...
	template <typename T>
	static T&& declval();

	template <typename... Args>
	using TheRefinedProof =
		decltype(TheRefined::constrained(declval<Constraint>(), declval<Args>()...));

	template <typename... Args>
	using ConstraintProof =
		decltype(Constraint::constrain(declval<TheRefined>(), declval<Args>()...));

public:
	constexpr inline const TheRefined& value() { return x; }

	// Allow "passive" satisfaction of constraints if both the constrained and the constraint agree
	// (by use of declared-but-undefined static members)
	// that a certain set of input types would make TheConstrained satisfy Constraint right out the gate
	// (and that TheConstrained is naturally constructable from those inputs anyway).
	// The definitions of the "proofs" above require that the static member functions
	// on TheConstrained and Constraint respectively "accept" the other (via a declval parameter),
	// establishing a sort of "two-way friend" relationship for refinement purposes.
	template <
		typename... Args,
		typename = decltype(TheRefined(declval<Args>()...)),
		typename = TheRefinedProof<Args...>,
		typename = ConstraintProof<Args...>
	>
	constexpr inline Refined(Args&&... args):
		x(static_cast<Args&&>(args)...)
	{
		// Constructor template arguments can never be explicitly specified,
		// no need to worry about any "hacks" of the last two guards.
		// the constructor call has to make sense in any case.
	}
};






template <unsigned int N>
struct at_most;

struct strvec {
	const char* data;
	unsigned int len;

	template <unsigned int N>
	constexpr strvec(const char (&data)[N]): data(data), len(N) {}

	template <
		unsigned int M,
		unsigned int L,
		typename = typename ds2::microstl::type_traits::enable_if<L <= M>::type
	>
	static void constrained(at_most<M>, const char (&data)[L]);
};

// example: an array at most N elements in size.
template <unsigned int N>
struct at_most {
	template <
		unsigned int L,
		typename = ds2::microstl::type_traits::enable_if_t<L <= N>
	>
	static void constrain(strvec, const char (&data)[L]);
};

// sigh. see is_detected.cpp, we're stuck with boilerplating this ourselves.
template <typename T, typename... Args>
constexpr bool can_construct_(...) { return false; }
template <typename T, typename... Args, typename = decltype(T(ds2::microstl::declval<Args>()...))>
constexpr bool can_construct_(int) { return true; }
template <typename... Args>
constexpr bool can_construct = can_construct_<Args...>(0);

static_assert(can_construct<Refined<strvec, at_most<5>>, char[4]>);
// kaboom: no overload present to construct this.
static_assert(not can_construct<Refined<strvec, at_most<5>>, char[6]>);



// let's try something else, type-level values.
// if we wanted to have e.g. a Refined<unsigned, less_than<5>>,
// there's not an easy to allow that at compile time via the passive;
// we'd already have to have a less_than<5> value or stronger!
// there are two ways to solve this:
// pass the value in a type-level literal, or implement a dynamic throwing constructor
// (we can still have a throw in a constexpr context, it's just not as nice at producing errors).
// TODO...




