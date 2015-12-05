#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

#include "common.h"

#define RESTRICT_ROOT	1
#define ADMIN_PASSWD_LENGTH	16

void print_usage() {
	printf("Usage: vxque [OPTION]\n\n");
	printf(" -c, --config       Specify config\n");
	printf(" -h, --help         This help\n");
}

int main(int argc, char *argv[]) {
	int opt_config = 0;
	char configname[1024];
	json_value *config = NULL;

#ifdef RESTRICT_ROOT
	uid_t uid = getuid(), euid = geteuid();
	if (uid != 0 || uid != euid) {
		fprintf(stderr, "User must have root privileges\n");
		return 1;
	}
#endif

	static struct option long_options[] = {
		{"config",       required_argument, 0,  'c' },
		{"help",         no_argument,       0,  'h' },
		{0,              0,                 0,   0  }
	};

	int opt, long_index = 0;
	while ((opt = getopt_long(argc, argv,"c:h", long_options, &long_index)) != -1) {
		switch (opt) {
			case 'c' :
				strncpy(configname, optarg, 1024);
				opt_config = 1;
				break;
			default:
				print_usage(); 
				return 1;
		}
	}

	if (!opt_config) {
		strncpy(configname, "/etc/vxpanel.conf", 1024);
	}

	config = parse_config(configname);
	if (!config)
		return 1;

	json_value_free(config);
	return 0;
}
