#include <ds2/microstl/utility/forward.hpp>
using namespace ds2::microstl::utility::forward;

struct S {};
void l(S&);
void r(S&&);

// non-forwarding and where it blows up...
void tl(S& s) { l(s); }
//void tr(S&& s) { r(s); } // bang

template <typename F, typename... Args>
void wrap(F&& f, Args&&... args) {
	f(forward<Args>(args)...);
} 

void tl2(S& s) { wrap(l, s); }
// yet another layer of the "rvalue decay" annoyance...
void tr2(S&& s) { wrap(r, forward<S&&>(s)); }

