extern "C" {
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <openssl/sha.h>
}



template <size_t N>
void fatal(const char (&msg)[N]) {
	// this trick gets us the entire literal array including the implied nul terminator,
	// so just be sure to strip that off.
	static_assert(N > 0);
	write(2, msg, N-1);
}



/*
argh, giving C better error handling via macros, what next,
s-exprs? ... oh wait. >:D
Soon(tm)

anyway, openssl returns boolean-like integer codes without errno.
1 is _success_.
*/
#define CHECK_s(expr, msg) \
	r = expr; \
	if (!r) { \
		fatal(msg); \
		return 1; \
	}

/*
some other bits below use errno style returns, where -1 is error,
and errno holds the actual problem.
*/
#define CHECK_e(expr, msg) \
	r = expr; \
	if (r < 0) { \
		fatal(msg); \
		return errno; \
	}


static const constexpr size_t bufsz = 4096;
static const constexpr size_t hashsz = SHA256_DIGEST_LENGTH;
int main(int argc, char** argv) {
	SHA256_CTX ctx;
	long r;
	ssize_t bytes_read;
	size_t bytes_total = 0;
	static unsigned char readmem[bufsz];
	unsigned char digest[hashsz];

	/*
	NB: file accessed via stdin but still needs to be seekable.
	insanity_vcs always operates on files in the store anyhow,
	so this isn't an issue.
	this is just to ensure we're not passed a piped file by accident,
	as by definition such a file would then not be saved anywhere.
	that shouldn't happen in the use case of an object store,
	it should be staged already.
	*/
	CHECK_e( lseek(0, 0, SEEK_SET), "seek failed\n");

	CHECK_s( SHA256_Init(&ctx), "SHA256_Init() failed\n");

	while ( (bytes_read = read(0, readmem, bufsz)) > 0) {
		CHECK_s( SHA256_Update(&ctx, readmem, bytes_read), "SHA256_Update() failed\n");
		bytes_total += bytes_read;
		// overflow is fairly easy to spot as ssize_t can never have it's high bit set 
		// (without being an error anyway).
		// so we can always spot that happening to our counter before it actually does.
		if (((ssize_t)bytes_read) < 0) {
			fatal("counter overflow");
			return 1;
		}
	}

	// loop terminates on either EOF (read_bytes = 0) or error (< 1),
	// so just check that here...
	CHECK_e( bytes_read, "read failed\n");

	CHECK_s( SHA256_Final(digest, &ctx), "SHA256_Final() failed\n");

	// for now write it to stdout in binary; hex can come later.
	CHECK_e( write(1, digest, hashsz), "stdout hash write failed\n");

	return 0;
}

