#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include "json.h"
#include "builder.h"

json_value *parse_config(char *filename);
void set_key_config(char *configname, json_value *config, char *key);
int verify_config(json_value *config);
void *get_random_data(size_t len);
int reset_admin_password(json_value *config, char *passwd);

#endif // COMMON_H_INCLUDED
