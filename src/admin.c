#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <mysql/mysql.h>

#include "json.h"

#define QUERY_SZ	1024

int reset_admin_password(json_value *config, char *passwd) {
	char query[QUERY_SZ];
	char *db_user = NULL;
	char *db_password = NULL;
	char *db_database = NULL;
	const char *sql = "UPDATE `user` SET password = ENCRYPT('%s', CONCAT('$6$', SUBSTRING(SHA(RAND()), -16))) "
					  "WHERE username='admin' and active=1";

	MYSQL *conn = mysql_init(NULL);

	for (unsigned int x = 0; x < config->u.object.length; ++x) {
		if (!strcmp(config->u.object.values[x].name, "db")) {
			for (unsigned int y = 0; y < config->u.object.values[x].value->u.object.length; ++y) {
				if (!strcmp(config->u.object.values[x].value->u.object.values[y].name, "user")) {
					db_user = config->u.object.values[x].value->u.object.values[y].value->u.string.ptr;
				} else if (!strcmp(config->u.object.values[x].value->u.object.values[y].name, "database")) {
					db_database = config->u.object.values[x].value->u.object.values[y].value->u.string.ptr;
				} else if (!strcmp(config->u.object.values[x].value->u.object.values[y].name, "password")) {
					db_password = config->u.object.values[x].value->u.object.values[y].value->u.string.ptr;
				}
			}
		}
	}

	if (!db_user || !db_password || !db_database) {
		fprintf(stderr, "Database options not set\n");
		return 0;
	}

	if (!conn) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		return 0;
	}

	if (!mysql_real_connect(conn, "localhost", db_user, db_password, db_database, 0, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		mysql_close(conn);
		return 0;
	}

	snprintf(query, QUERY_SZ, sql, passwd);
	if (mysql_query(conn, query)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		mysql_close(conn);
		return 0;
	}

	mysql_close(conn);
	return 1;
}
