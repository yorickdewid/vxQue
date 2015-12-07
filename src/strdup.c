#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

char *strdup(const char *str) {
	size_t len;
	char *copy = NULL;

	assert(str != NULL);

	len = strlen(str) + 1;
	if (!(copy = malloc(len)))
		return (NULL);
	memcpy(copy, str, len);
	return (copy);
}