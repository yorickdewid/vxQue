#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

char *add_user(json_value *param) {
	char cmd[1024];
	char *username = NULL;

	if (param->type != json_object)
		return NULL;

	if (!strcmp(param->u.object.values[0].name, "user")) {
		printf("Creating new user %s\n", param->u.object.values[0].value->u.string.ptr);
		username = param->u.object.values[0].value->u.string.ptr;
	}

	strcpy(cmd, "useradd -s /bin/bash ");
	strcat(cmd, username);
	strcat(cmd, " 2>&1");

	FILE *cfp = popen(cmd, "r");
	if (!cfp) {
		return NULL;
	}

	char *rs = (char *)malloc(1024);
	while (fgets(rs, 1024, cfp) != NULL);

	pclose(cfp);
	return rs;
}

char *delete_user(json_value *param) {
	char cmd[1024];
	char *username = NULL;

	if (param->type != json_object)
		return NULL;

	if (!strcmp(param->u.object.values[0].name, "user")) {
		printf("Deleting user %s\n", param->u.object.values[0].value->u.string.ptr);
		username = param->u.object.values[0].value->u.string.ptr;
	}

	strcpy(cmd, "userdel -r ");
	strcat(cmd, username);
	strcat(cmd, " 2>&1");

	FILE *cfp = popen(cmd, "r");
	if (!cfp) {
		return NULL;
	}

	char *rs = (char *)malloc(1024);
	while (fgets(rs, 1024, cfp) != NULL);

	pclose(cfp);
	return rs;
}
