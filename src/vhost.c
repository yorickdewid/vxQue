#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>

#include "common.h"

#define APACHE_LOC	"/usr/local/vxpanel/srv/apache"

char *create_vhost(json_value *param, int *success) {
	char *username = NULL;
	char *email = NULL;
	char *host = NULL;

	if (param->type != json_object)
		return "Invalid parameters";

	for (unsigned int x = 0; x < param->u.object.length; ++x) {
		if (!strcmp(param->u.object.values[x].name, "user")) {
			username = param->u.object.values[x].value->u.string.ptr;
		} else if (!strcmp(param->u.object.values[x].name, "host")) {
			host = param->u.object.values[x].value->u.string.ptr;
		} else if (!strcmp(param->u.object.values[x].name, "email")) {
			email = param->u.object.values[x].value->u.string.ptr;
		}
	}

	if (!username || !email || !host) {
		return "Not all parameters are given";
	}

	printf("Setup vhost for domain %s under user %s with email %s\n", host, username, email);

	if (!getpwnam(username)) {
		return "User does not exist";
	}

	int rtn = 0;
	rtn += invoke_exec("cp " APACHE_LOC "/vhost.template " APACHE_LOC "/vhost.d/%s.conf", host);
	rtn += invoke_exec("mkdir -p /home/%s/www/%s", username, host);
	rtn += invoke_exec("sed -i \"s/CHANGEME_EMAIL/%s/g\" " APACHE_LOC "/vhost.d/%s.conf", email, host);
	rtn += invoke_exec("sed -i \"s/CHANGEME_USER/%s/g\" " APACHE_LOC "/vhost.d/%s.conf", username, host);
	rtn += invoke_exec("sed -i \"s/CHANGEME_HOST/%s/g\" " APACHE_LOC "/vhost.d/%s.conf", host, host);
	rtn += invoke_exec("service httpd reload");

	if (rtn > 0) {
		return "Vhost setup failed";
	}
	
	*success = 1;
	return "Vhost created and loaded";
}

char *delete_vhost(json_value *param, int *success) {
	char *username = NULL;
	char *host = NULL;

	if (param->type != json_object)
		return "Invalid parameters";

	for (unsigned int x = 0; x < param->u.object.length; ++x) {
		if (!strcmp(param->u.object.values[x].name, "user")) {
			username = param->u.object.values[x].value->u.string.ptr;
		} else if (!strcmp(param->u.object.values[x].name, "host")) {
			host = param->u.object.values[x].value->u.string.ptr;
		}
	}

	if (!username || !host) {
		return "Not all parameters are given";
	}

	printf("Delete vhost for domain %s under user %s\n", host, username);

	if (!getpwnam(username)) {
		return "User does not exist";
	}

	int rtn = 0;
	rtn += invoke_exec("rm -f " APACHE_LOC "/vhost.d/%s.conf", host);
	rtn += invoke_exec("rm -rf /home/%s/www/%s", username, host);
	rtn += invoke_exec("rm -f /home/%s/logs/%s_access.log", username, host);
	rtn += invoke_exec("rm -f /home/%s/logs/%s_error.log", username, host);
	rtn += invoke_exec("service httpd reload");

	if (rtn > 0) {
		return "Vhost deletion failed";
	}
	
	*success = 1;
	return "Vhost and related files deleted";
}
