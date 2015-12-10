#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include "json.h"

char *strdup(const char *str);
int invoke_exec(const char *command, ...);

json_value *parse_config(char *filename);
int db_init(json_value *config);
int submitted_jobs();
void update_job_pending(char *id);
void update_job_rejected(char *id, char *result);
void update_job_done(char *id, int success, char *result);
void db_close();

int handle_action(char *id, char *action, char *param_object);

char *add_user(json_value *param, int *success);
char *delete_user(json_value *param, int *success);
char *create_vhost(json_value *param, int *success);
char *delete_vhost(json_value *param, int *success);
char *verify_config();
char *send_mail(json_value *param, int *success);

#endif // COMMON_H_INCLUDED
