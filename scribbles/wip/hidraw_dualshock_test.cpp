extern "C" {
// open, O_RDONLY
#include <fcntl.h>
// read, write (ssize_t, size_t)
#include <unistd.h>
// errno (ugh)
#include <errno.h>
}
// strlen (size_t)
#include <cstring>
// uint8_t
#include <cstdint>
// CHAR_BIT
#include <climits>
// std::{make_signed, make_unsigned}
#include <type_traits>

typedef uint8_t u8;
typedef uint16_t u16;

typedef int16_t i16;
typedef int8_t i8;


inline int bail(const char* msg, size_t s) {
	// oh bloody hell. a void cast ain't enough sometimes?
	// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66425
	ssize_t stfu_gcc = write(2, msg, s);
	(void)stfu_gcc;
	return errno;
}

#define BUFSZ 4096
static u8 buf[BUFSZ];

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











// alright, can I has a proper structs for this already...
// the dualshock 4 HID data uses biased representations a lot,
// whereby it's an unsigned field representing a value which is field_value - (2^(bitwidth - 1)).
// furthermore like a lot of HID reports the bytes are packed and unaligned.
// the C++ standard therefore dictates we must copy the bytes into a properly aligned variable,
// instead of trying to do a direct unaligned load (I don't care if x86 can do that).
// for the most part, the fields are little endian.
// (I wonder if they did this because the PS4 is an x86 system. any bets they do unaligned loads?)

// thusly, let us first define some helper types.
// XXX SCRIPT: candidate-header-separation START
template <size_t N>
struct unaligned_field_base {
	u8 bytes[N];
};

struct unaligned_u8: public unaligned_field_base<1> {
	constexpr u8 load_raw() const { return bytes[0]; }
};

static constexpr u8 byte_bits = CHAR_BIT;

template <typename T>
struct load_unaligned_helper: public unaligned_field_base<sizeof(T)> {
	constexpr static size_t width = sizeof(T);

	constexpr T load_raw() const {
		T result = 0;
		for (size_t i = 0; i < width; i++) {
			if constexpr (width > 1) result <<= byte_bits;
			// XXX: am I being dumb here, surely we should not need the this->. part
			// surely bytes should be in scope as a (superclass) data member...
			// see unaligned_u8 above, that works fine! >:(
			result |= this->bytes[i];
		}
		return result;
	}
};

struct unaligned_u16: load_unaligned_helper<uint16_t> {};

template <typename T>
struct biased: public load_unaligned_helper<std::make_unsigned_t<T>> {
private:
	static_assert(
		std::is_signed<T>::value,
		"biased representation only makes sense for signed integrals.");

	using raw_type = std::make_unsigned_t<T>;
	// ugh, where's my damn super keyword already, it's unambiguous damnit!
	using super = load_unaligned_helper<raw_type>;
	constexpr static auto width = super::width;
	constexpr static unsigned bias = 2 << ((width * byte_bits) - 1);
public:

	constexpr T load() const {
		raw_type value = this->load_raw();
		// do the wrap into negative integers so as to ensure it's always legal.
		// signed wrap-around wasn't until C++20 AFAIK.
		value -= bias;
		return static_cast<T>(value);
	}
};

struct joystick {
	biased<i8> x;
	biased<i8> y;	// NB: +Y is down
};



struct main_report {
	// NB: report ID not specified here, it's handled elsewhere.
	joystick joy_left;
	joystick joy_right;
};
static_assert(alignof(main_report) == 1, "packed report struct not single byte alignment.");
static_assert(sizeof(main_report) <= 63, "packed report struct is longer than the report body.");














constexpr bool is_negative(i8 in) {
	u8 raw = static_cast<u8>(in);
	return (raw >> 7) == 1;
}
constexpr u8 abs(i8 in) {
	u8 raw = static_cast<u8>(in);
	return (~raw) + 1;
}
inline void show_biased_i8(const biased<i8> in, char* buf4) {
	const i8 uv = in.load();
	buf4[0] = ' ';
	buf4[1] = ((is_negative(uv)) ? '-' : '+');
	u8 v = abs(uv);
	tohex(v, &buf4[2]);
}













static char joystick_l_showbuf[] = "# joystick  left .00 .00\n";

template <typename T, size_t N>
constexpr size_t sz(const T(&)[N]) {
	return N;
}

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

		/*
		static char hexbuf[BUFSZ * 3];
		size_t outsize = hexline(buf, hexbuf, size);
		ssize_t written = write(1, hexbuf, outsize);
		if (written < 0) return bail("# !write\n", 9);
		if ((size_t)written != outsize) return bail("# !write:short\n", 15);
		*/

		if ((size == 64) && (buf[0] == '\x01')) {
			const auto& report = *(const main_report*)(&buf[1]);
			show_biased_i8(report.joy_left.x, &joystick_l_showbuf[16]);
			show_biased_i8(report.joy_left.y, &joystick_l_showbuf[20]);
			(void)write(1, joystick_l_showbuf, sz(joystick_l_showbuf));

			/*
			static char imu_showbuf[26] = "# gyro +0000 +0000 +0000\n";

			char* showbuf = imu_showbuf;
			show_s16le(&buf[13], &showbuf[6]);
			show_s16le(&buf[15], &showbuf[12]);
			show_s16le(&buf[17], &showbuf[18]);
			ssize_t written = write(1, showbuf, 25);
			(void)written;
			*/
		}
	}
}

