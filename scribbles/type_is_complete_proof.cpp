#include <cstddef>

struct S;
// this must be uncommented to make S complete and allow the below to compile.
//struct S {};


template <size_t S> struct size_proof {};

template <typename T>
struct type_is_complete_now {
	// this is the detonator: sizeof(incomplete type) is ill-formed
	// (as one would expect - how can you know it's size without knowing what's in it?)
	using proof = size_proof<sizeof(T)>;
};

struct F {
	// it seems gcc can be a bit clever with the above if we simply have a "using" here,
	// so force the type to be fully "concrete" by having it be a (static) member.
	constexpr static auto proof = typename type_is_complete_now<S>::proof();
};

// we can uncomment this and it would have no effect
//struct S {};




