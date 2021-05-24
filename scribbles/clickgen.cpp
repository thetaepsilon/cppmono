#include <cstdio>
#include <cstddef>
#include <cerrno>

typedef unsigned char u8;

constexpr size_t bufsz = 4096;
constexpr u8 threshold = 0x00;
u8 buffer[bufsz];

int main(void) {
	size_t size;
	while ((size = fread(static_cast<void*>(buffer), 1, bufsz, stdin))) {
		for (size_t i = 0; i < size; i++) {
			const u8 input = buffer[i];
			// turn output pulse on when (random) input is lte. our rarity threshold.
			// output for unsigned bytes in this case.
			const u8 result = (input <= threshold) ? 0xFF : 0x00;
			putchar(result);
			// output low value immediately after so that pulses can't merge.
			putchar(0x00);
		}
	}
	return errno;
}
