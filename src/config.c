#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "json.h"

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

	json_value *config = json_parse((json_char *)file_contents, file_size);
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

int verify_config() {
	return 1;
}