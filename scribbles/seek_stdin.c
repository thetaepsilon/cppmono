#include <unistd.h>
#include <errno.h>

int main(void) {
	int ret = 0;
	#define stdin 0
	if (lseek(stdin, 0, SEEK_SET) < 0) {
		write(2, "# !lseek\n", 9);
		ret = errno;
	}
	#undef stdin
	return ret;
}
