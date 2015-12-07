#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include "json.h"

json_value *parse_config(char *filename);
int db_init(json_value *config);
int submitted_jobs();
void update_job_pending(char *id);
void update_job_rejected(char *id);
void db_close();

int handle_action(char *id, char *action, char *param_object);

int add_user(json_value *param);
int delete_user(json_value *param);
int create_vhost(json_value *param);
int verify_config();
int send_mail(json_value *param);

#endif // COMMON_H_INCLUDED
