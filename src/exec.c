//#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "common.h"

int invoke_exec(const char *command, ...) {
	char cmd[1024];

	va_list list;
	va_start(list, command);
	vsprintf(cmd, command, list);

	return system(cmd);

	/*FILE *cfp = popen(cmd, "r");
	if (!cfp) {
		return NULL;
	}

	char *rs = (char *)malloc(1024);
	while (fgets(rs, 1024, cfp) != NULL);

	pclose(cfp);
	va_end(list);
	return rs;*/
	return NULL;
}