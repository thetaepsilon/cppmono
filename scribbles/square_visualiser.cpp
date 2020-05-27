#include <cstddef>
extern "C" {
#include <unistd.h>
}

// returns the consumed number of elements.
// may be less than size, doesn't infill remaining elements.
// indexes which are a square number are set to on, all others off.
template <typename T>
constexpr std::size_t fill_buffer(T* target, std::size_t size, T on, T off) {
	std::size_t last_square = 0;
	std::size_t base = 0;
	std::size_t squared = 0;
	std::size_t consumed = 0;

	// beware the off-by-one errors! even index 0 needs at least one element.
	while (squared < size) {
		// fill from last written square to current one with off-state.
		// note starts at one, as we'll write the next square out as the "end" of that run.
		for (std::size_t n = 1; n < (squared - last_square); n++) {
			target[consumed++] = off;
		}
		target[consumed++] = on;

		base++;
		// XXX: overflows, but who's seriously going to run this up to 32 bits?
		// (probably me in the future for lols, oh well)
		last_square = squared;
		squared = base * base;
	}

	return consumed;
}


constexpr static const std::size_t bufsz = 200;
int main(void) {
	// the one time I could get away with static memory...
	static char buf[bufsz];
	// just reserve some room at the end for a newline...
	std::size_t used = fill_buffer(buf, bufsz-1, '*', ' ');
	buf[used++] = '\n';

	// XXX: no error checking...
	write(1, buf, used);
}

