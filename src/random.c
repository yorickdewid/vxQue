#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

void *get_random_data(size_t len) {
	int fd_rand = open("/dev/random", O_RDONLY);

	if (!len)
		len = 20;

	char *rand_data = (char *)malloc(len);
	if (!rand_data) {
		fprintf(stderr, "Memory error: unable to allocate %zu bytes\n", len);
		return NULL;
	}

	//size_t rand_len = len;
	//while (rand_len < 48) {
	ssize_t result = read(fd_rand, rand_data, len);
	if (result < 0) {
		return NULL;
	}
	//rand_len += result;
	//}
	close(fd_rand);

	return rand_data;
}