#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

void die() {
	exit(errno);
}

/* NB: all seek and read operations are done via provided stdin, hence hardcoded FD */
off_t assert_seek(off_t offset, int whence, char* error, size_t len) {
	off_t ret = lseek(0, offset, whence);
	if (ret < 0) {
		write(2, error, len);
		die();
	}
	return ret;
}

int main(int argc, char** argv) {
	/*
	NB: file accessed via stdin but still needs to be seekable,
	as first start/end bytes are read as well as length being determined.
	*/
	assert_seek(0, SEEK_SET, "seek to read header failed");
}

