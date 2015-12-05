#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "json.h"
#include "builder.h"

json_value *parse_config(char *filename) {
	struct stat filestatus;
	FILE *fp = NULL;

	if (stat(filename, &filestatus) != 0) {
		fprintf(stderr, "Config file %s not found\n", filename);
		return NULL;
	}

	size_t file_size = filestatus.st_size;
	char *file_contents = (char *)malloc(filestatus.st_size);
	if (!file_contents) {
		fprintf(stderr, "Memory error: unable to allocate %zu bytes\n", file_size);
		return NULL;
	}

	fp = fopen(filename, "rt");
	if (!fp) {
		fprintf(stderr, "Unable to open config %s\n", filename);
		fclose(fp);
		free(file_contents);
		return NULL;
	}
	if (fread(file_contents, file_size, 1, fp) == 0 ) {
		fprintf(stderr, "Unable t read content of config %s\n", filename);
		fclose(fp);
		free(file_contents);
		return NULL;
	}
	fclose(fp);

	json_settings settings;
	memset(&settings, 0, sizeof(json_settings));
	settings.value_extra = json_builder_extra;

	char error[128];
	json_value *config = json_parse_ex(&settings, (json_char *)file_contents, file_size, error);
	if (!config) {
		fprintf(stderr, "Unable to parse config\n");
		free(file_contents);
		return NULL;
	}
	free(file_contents);

	if (config->type != json_object) {
		fprintf(stderr, "Unable to parse config\n");
		json_value_free(config);
		return NULL;
	}

	return config;
}

void set_key_config(char *configname, json_value *config, char *key) {
		for (unsigned int x = 0; x < config->u.object.length; ++x) {
			if (!strcmp(config->u.object.values[x].name, "secret")) {
				free(config->u.object.values[x].value->u.string.ptr);
				config->u.object.values[x].value->u.string.ptr = key;
				config->u.object.values[x].value->u.string.length = 40;
			}
		}

		json_serialize_opts opts;
		opts.mode = json_serialize_mode_multiline;
		opts.opts = json_serialize_opt_use_tabs;
		opts.indent_size = 1;

		char *buf = malloc(json_measure_ex(config, opts));
		json_serialize_ex(buf, config, opts);

		unlink(configname);
		FILE *fp = fopen(configname, "ab");
		if (fp) {
			fputs(buf, fp);
			fclose(fp);
		}

		free(buf);
}

int verify_config(json_value *config) {
	int ok = 1;
	int _db_name = 0;
	int _db_user = 0;
	int _secret = 0;
	int _port = 0;

	for (unsigned int x = 0; x < config->u.object.length; ++x) {
		if (!strcmp(config->u.object.values[x].name, "db")) {
			for (unsigned int y = 0; y < config->u.object.values[x].value->u.object.length; ++y) {
				if (!strcmp(config->u.object.values[x].value->u.object.values[y].name, "user")) {
					if (!config->u.object.values[x].value->u.object.values[y].value->u.string.length) {
						fprintf(stderr, "> db.user is not set [FATAL]\n");
						ok = 0;
					} else
						_db_user = 1;
				} else if (!strcmp(config->u.object.values[x].value->u.object.values[y].name, "database")) {
					if (!config->u.object.values[x].value->u.object.values[y].value->u.string.length) {
						fprintf(stderr, "> db.database is not set [FATAL]\n");
						ok = 0;
					} else {
						_db_name = 1;
					}
				}
			}
			if (!_db_user){
				fprintf(stderr, "> db.user does not exist [FATAL]\n");
				ok = 0;
			}
			if (!_db_name) {
				fprintf(stderr, "> db.database does not exist [FATAL]\n");
				ok = 0;
			}
		} else if (!strcmp(config->u.object.values[x].name, "secret")) {
			if (config->u.object.values[x].value->u.string.length != 40) {
				fprintf(stderr, "> secret is not set or invalid [FATAL]\n");
				ok = 0;
			} else {
				_secret = 1;
			}
		} else if (!strcmp(config->u.object.values[x].name, "service")) {
			for (unsigned int y = 0; y < config->u.object.values[x].value->u.object.length; ++y) {
				if (!strcmp(config->u.object.values[x].value->u.object.values[y].name, "port")) {
					if (!config->u.object.values[x].value->u.object.values[y].value->u.string.length) {
						fprintf(stderr, "> service.port is not set [FATAL]\n");
						ok = 0;
					} else if (config->u.object.values[x].value->u.object.values[y].value->u.string.length > 65535) {
						fprintf(stderr, "> service.port is invalid [FATAL]\n");
						ok = 0;
					} else
						_port = 1;
				}
			}
		}
	}

	if (!_secret) {
		fprintf(stderr, "> secret does not exist [FATAL]\n");
		ok = 0;
	}
	if (!_port) {
		fprintf(stderr, "> service.port does not exist [FATAL]\n");
		ok = 0;
	}

	return ok;
}
