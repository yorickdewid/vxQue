#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <mysql/mysql.h>

#include "common.h"

#define QUERY_SZ	1024

static MYSQL *conn = NULL;

int db_init(json_value *config) {
	char *db_user = NULL;
	char *db_password = NULL;
	char *db_database = NULL;

	conn = mysql_init(NULL);

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
		conn = NULL;
		return 0;
	}

	if (!conn) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		conn = NULL;
		return 0;
	}

	if (!mysql_real_connect(conn, "localhost", db_user, db_password, db_database, 0, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		mysql_close(conn);
		conn = NULL;
		return 0;
	}

	return 1;
}
	
int submitted_jobs() {
	char query[QUERY_SZ];
	const char *sql = "SELECT qid as jobid,action,params,created as submitted FROM queue WHERE status='SUBMITTED' AND started IS NULL AND finished IS NULL";

	if (!conn)
		return 0;

	snprintf(query, QUERY_SZ, sql);
	if (mysql_query(conn, query)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		mysql_close(conn);
		return 0;
	}

	MYSQL_RES *result = mysql_store_result(conn);
	if (!result)
		return 0;

	int num_fields = mysql_num_fields(result);

	MYSQL_ROW row;
	MYSQL_FIELD *field = NULL;

	while ((field = mysql_fetch_field(result))) {
		printf("%s\t", field->name);
	}

	printf("\n-------------------------------\n");

	while ((row = mysql_fetch_row(result))) { 
		for(int i = 0; i < num_fields; ++i) { 
			printf("%s\t", row[i] ? row[i] : "NULL\t");
		}
		printf("\n");
	}
	printf("\n");

	mysql_data_seek(result, 0);
	while ((row = mysql_fetch_row(result))) { 
		for(int i = 0; i < num_fields; ++i) { 
			if (i == 0) {
				if (row[i]) {
					handle_action(row[i], row[i + 1], row[i + 2]);
				}
			}
		}
	}

	mysql_free_result(result);
	return 1;
}

void db_close() {
	if (conn)
		mysql_close(conn);
}
