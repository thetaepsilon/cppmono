// here's a function template. (could also be a class)
template <typename T>
constexpr T func(T a, T b, T c);

// let's say that we want to assert something symbolic about func.
// for a contrived example, let's say it must return (a + b) * c.
// normally, this might operate on integers.
// however, we're not satisfied with testing specific values.
// oh no, we want to assert this property is *universally* true.
// as func is a template, we could check this by making T something else,
// say a struct that "remembers" symbolic operations performed on it.

struct symbolic {
	// note: in any ordinary runtime situation this would be a dangling pointer fest.
public:
	enum class op {
		add,
		multiply,
		constant
	} operation;
	const symbolic& opand1;
	const symbolic& opand2;

	constexpr symbolic(): operation(op::constant), opand1(*this), opand2(*this) {}
	constexpr symbolic(op oper, const symbolic& a1, const symbolic& a2):
		operation(oper), opand1(a1), opand2(a2) {}
};
constexpr symbolic& operator*(const symbolic& that, const symbolic& other) {
	return *(new symbolic(symbolic::op::multiply, that, other));
}
constexpr symbolic& operator+(const symbolic& that, const symbolic& other) {
	return *(new symbolic(symbolic::op::add, that, other));
}

// a trivial implementation of func (again contrived, but stick with me here)
template <typename T>
constexpr T func(T a, T b, T c) {
	return (a + b) * c;
}

// to test func, we call it instantiated for symbolic, and inspect the expression tree.
// throws are used here so we can see what fails.
constexpr bool test() {
	const symbolic a, b, c;
	const symbolic& r = func<const symbolic&>(a, b, c);

	// r should be: ((a + b) * c)
	// TODO: this could be a tree comparison, too.
	// comparing constants would be done by pointer equality,
	// and all other operations would be structurally compared recursively
	// (also checking the operation for each tree node).
	if (r.operation != symbolic::op::multiply) throw "not multiply";
	if (&r.opand2 != &c) throw "not c";

	const symbolic& i = r.opand1;
	if (i.operation != symbolic::op::add) throw "not add";
	if (&i.opand1 != &a) throw "not a";
	if (&i.opand2 != &b) throw "not b";

	// note that because symbolic dyna-allocs,
	// C++20 requires we throw those away before allowing a constexpr function to end.
	// in the happy case this is fine as the structure should be what we expect above,
	// and in fact this check catches any unexpected structure too
	// (such as a sneaky extra operation instead of c on the RHS of the multiply).
	delete &r.opand1;
	delete &r;

	return true;
}

// result?
static_assert(test());

