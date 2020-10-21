extern "C" {
#define _POSIX_C_SOURCE 199309L
#include <time.h>
#include <errno.h>
#include <unistd.h>
}
#include <cstdio>
#include <cstdint>

constexpr unsigned int one_sec_ns = 1000000000;
constexpr timespec operator-(const timespec& later, const timespec& earlier) {
	auto seconds = later.tv_sec - earlier.tv_sec;
	auto nanoseconds = later.tv_nsec - earlier.tv_nsec;
	// nanoseconds could wind up negative if e.g.
	// later was 3s 0ns and earlier was 2s 900,000,000ns.
	// correct that here by subtracting time from seconds for every 1billion ns.
	// (XXX: nanoseconds from each *individual* clock_gettime() timespec is always normalised... right!?)
	if (nanoseconds < 0) {
		nanoseconds += one_sec_ns;
		seconds -= 1;
	}
	return timespec { seconds, nanoseconds };
}

inline timespec gettime() {
	timespec ret;
	int r = clock_gettime(CLOCK_MONOTONIC_RAW, &ret);
	if (r < 0) throw errno;
	return ret;
}


int main(void) {
	puts("# waiting... press enter to the beat");

	constexpr size_t bufsz = 4096;
	static char blackhole[bufsz];

	// compiler shut up, I know I haven't initialised this on the first loop! *sigh*
	auto previous = timespec { 0, 0 };
	bool has_previous = false;

	while (read(0, blackhole, bufsz) >= 0) {
		auto current = gettime();
		if (has_previous) {
			auto delta = current - previous;
			printf("%lds %ldns\n", delta.tv_sec, delta.tv_nsec);
			//printf("# prev %lds %ldns\n", previous.tv_sec, previous.tv_nsec);

			// as long as seconds isn't particularly large,
			// we can combine the delta into an int64 number of nanoseconds
			// (specifically, multiplying seconds into seconds shifts left by ~30 bits).
			// however, we also shortly want to perform bpm calculation.
			// limit seconds to a sane number to prevent issues.
			if (delta.tv_sec > 59) {
				puts("# timeout");
				return 0;
			}
			int64_t ns = (delta.tv_sec * one_sec_ns) + delta.tv_nsec;
			if (ns < 0) throw "timer negative??";

			// rate, cycles per second = 1 / period in seconds
			// hence we can also work from nanoseconds,
			// specifically 1,000,000,000 / period in nanoseconds.
			// obviously then the beats per minute is 60 * that.
			double bpm = (60.0 * one_sec_ns) / ns;
			printf("%.2lf\n", bpm);
		} else {
			puts("# first read");
			has_previous = true;
		}
		//printf("# current %lds %ldns\n", current.tv_sec, current.tv_nsec);
		fflush(stdout);
		previous = current;
	}
	puts("# read fail");
	return errno;
}


