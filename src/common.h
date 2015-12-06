#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include "json.h"

static int verbose = 0;

json_value *parse_config(char *filename);
int db_init(json_value *config);
int submitted_jobs();
void db_close();

#endif // COMMON_H_INCLUDED
