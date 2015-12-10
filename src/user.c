#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>

#include "common.h"

char *add_user(json_value *param, int *success) {
	char *username = NULL;

	if (param->type != json_object)
		return "Invalid parameters";

	if (!strcmp(param->u.object.values[0].name, "user")) {
		printf("Creating new user %s\n", param->u.object.values[0].value->u.string.ptr);
		username = param->u.object.values[0].value->u.string.ptr;
	}

	if (!username) {
		return "Not all parameters are given";
	}

	if (getpwnam(username)) {
		return "User already exists";
	}

	int rtn = 0;
	rtn += invoke_exec("useradd -s /bin/bash %s ", username);
	rtn += invoke_exec("mkdir -p /home/%s/bin", username);
	rtn += invoke_exec("mkdir -p /home/%s/etc", username);
	rtn += invoke_exec("mkdir -p /home/%s/www", username);
	rtn += invoke_exec("mkdir -p /home/%s/logs", username);
	rtn += invoke_exec("chown -R %s:%s /home/%s/*", username, username, username);
	rtn += invoke_exec("chmod +x /home/%s/", username);

	if (rtn > 0) {
		return "User setup failed";
	}
	
	*success = 1;
	return "User setup";
}

char *delete_user(json_value *param, int *success) {
	char *username = NULL;

	if (param->type != json_object)
		return "Invalid parameters";

	if (!strcmp(param->u.object.values[0].name, "user")) {
		printf("Deleting user %s\n", param->u.object.values[0].value->u.string.ptr);
		username = param->u.object.values[0].value->u.string.ptr;
	}

	if (!username) {
		return "Not all parameters are given";
	}

	if (!getpwnam(username)) {
		return "User does not exist";
	}

	int rtn = 0;
	rtn += invoke_exec("userdel -r %s ", username);

	if (rtn > 0) {
		return "User setup failed";
	}

	*success = 1;
	return "User deleted";
}
