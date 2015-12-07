#include <stdio.h>
#include <string.h>

#include "common.h"

enum action {
	USERADD,
	USERDEL,
	MAILSEND,
	CONFIGCHECK,
	CREATEVHOST
};

typedef struct {
	enum action action;
	char *name;
	int params;
	char *(* vfunc)(json_value *param);
} action_t;

action_t handler[] = {
	{USERADD,		"USERADD",		1, 	add_user},
	{USERDEL,		"USERDEL",		1,	delete_user},
	// {MAILSEND,		"MAILSEND",		1,	send_mail},
	// {CONFIGCHECK,	"CONFIGCHECK",	0,	verify_config},
	// {CREATEVHOST,	"CREATEVHOST",	1,	create_vhost},
};

int handle_action(char *id, char *action, char *param_object) {
	json_value *param = NULL;
	int execute = 0;

	printf("Accepting job #%s\n", id);
	printf("Job action: %s\n", action);

	if (param_object)
		printf("Job parameters: %s\n", param_object);

	if (param_object)
		param = json_parse((json_char *)param_object, strlen(param_object));

	for (unsigned int i = 0; i < sizeof(handler) / sizeof(action_t); ++i){
		if (!strcmp(handler[i].name, action)) {
			printf("Job executing\n");

			update_job_pending(id);

			char *rs = handler[i].vfunc(param);
			if (!rs)
				update_job_done(id, 0, NULL);
			else
				update_job_done(id, 1, rs);
			execute = 1;
			break;
		}
	}

	if (!execute) {
		printf("Job rejected: unknown action\n");
		update_job_rejected(id, strdup("unknown action"));
	}

	if (param)
		json_value_free(param);

	putchar('\n');
	return 1;
}