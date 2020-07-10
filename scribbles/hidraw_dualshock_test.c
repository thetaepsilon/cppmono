// open, O_RDONLY
#include <fcntl.h>
// read, write (ssize_t, size_t)
#include <unistd.h>
// strlen (size_t)
#include <string.h>
// errno (ugh)
#include <errno.h>
// uint8_t
#include <stdint.h>
// bool (welp, apparently legacy codebases clobbered this keyword)
#include <stdbool.h>
typedef uint8_t u8;


inline int bail(const char* msg, size_t s) {
	// oh bloody hell. a void cast ain't enough sometimes?
	// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66425
	ssize_t stfu_gcc = write(2, msg, s);
	(void)stfu_gcc;
	return errno;
}

#define BUFSZ 4096
static u8 buf[BUFSZ];
static char hexbuf[BUFSZ * 3];

const char hex[] = "0123456789ABCDEF";
// bugger printf. I don't need runtime format string parsing!
inline void tohex(u8 input, char* out) {
	out[0] = hex[input >> 4];
	out[1] = hex[input & 0x0FU];
}
inline void hexprint(u8* input, char* output, size_t count) {
	for (size_t i = 0; i < count; i++) {
		tohex(input[i], output);
		output += 2;
		*output++ = ' ';
	}
}
inline size_t hexline(u8* input, char* output, size_t count) {
	// use the last space char to replace it with a newline.
	size_t sz = count * 3;
	hexprint(input, output, count);
	if (sz > 0) {
		output[sz - 1] = '\n';
	}
	return sz;
}

// supposedly, the C standard doesn't assume signed representation.
// might as well be explicit about it here,
// because the dualshock's IMU data most certainly *is* two's complement.
inline bool is_u16_negative(uint16_t v) {
	return (v & 0x8000U);
}
inline bool is_u8_negative(u8 v) {
	return (v & 0x80U);
}
inline uint16_t abs_from_signed_u16(uint16_t input) {
	if (is_u16_negative(input)) {
		return ((~input) + 1);
	} else {
		return input;
	}
}
inline uint8_t abs_from_signed_u8(uint8_t input) {
	if (is_u8_negative(input)) {
		return ((~input) + 1);
	} else {
		return input;
	}
}
inline uint16_t read_u16_unaligned(const u8* data) {
	u8 lo = data[0];
	u8 hi = data[1];
	return (lo | (hi << 8));
}

// remember kids, not all the world is an x86 supporting unaligned accesses!
inline void show_s16le(const u8* data, char* buf6) {
	uint16_t uv = read_u16_unaligned(data);
	buf6[0] = ' ';
	buf6[1] = ((is_u16_negative(uv)) ? '-' : '+');
	uint16_t v = abs_from_signed_u16(uv);
	tohex(v >> 8, &buf6[2]);
	tohex(v & 0xFF, &buf6[4]);
}
inline void show_s8le(const u8 uv, char* buf4) {
	buf4[0] = ' ';
	buf4[1] = ((is_u8_negative(uv)) ? '-' : '+');
	u8 v = abs_from_signed_u8(uv);
	tohex(v, &buf4[2]);
}


// NB: this technically means we have a useless NUL here... oh well
  static char imu_showbuf[26] = "# gyro +0000 +0000 +0000\n";
//static char imu_showbuf[26] = "# gyro   +00   +00   +00\n";

int main(int argc, char** argv) {

	if (argc != 2) {
		const char* name = (argc > 0) ? argv[0] : "hidraw_test";
		size_t s = strlen(name);
		bail("# Usage: ", 10);
		bail(name, s);
		bail(" hidraw_device_path\n", 20);
		return 1;
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd < 0) return bail("# !open\n", 8);

	while (1) {
		ssize_t size = read(fd, buf, BUFSZ);
		if (size < 0) return bail("# !read\n", 8);

		size_t outsize = hexline(buf, hexbuf, size);
		ssize_t written = write(1, hexbuf, outsize);
		if (written < 0) return bail("# !write\n", 9);
		if (written != outsize) return bail("# !write:short\n", 15);

		if ((size == 64) && (buf[0] == '\x01')) {
			char* showbuf = imu_showbuf;
			show_s16le(&buf[13], &showbuf[6]);
			show_s16le(&buf[15], &showbuf[12]);
			show_s16le(&buf[17], &showbuf[18]);
			//show_s8le(buf[14], &showbuf[8]);
			//show_s8le(buf[16], &showbuf[14]);
			//show_s8le(buf[18], &showbuf[20]);
			written = write(1, showbuf, 25);
			(void)written;
		}
	}
}

