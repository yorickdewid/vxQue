#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

char *strmcon(char *str1, char *str2) {
	char *strconcat = (char *)malloc((strlen(str1) + strlen(str2) + 1) * sizeof(char));

	strcat(strconcat, str1);
	strcat(strconcat, str2);

	return strconcat;
}