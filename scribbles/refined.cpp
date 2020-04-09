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
public:
	// TheRefined() = delete;
	// beware the rule of five.
	
}

