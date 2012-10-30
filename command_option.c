#include "command_option.h"
#include "debugger.h"

int parse_command_option(int argc, char* argv[], COMMAND_OPTION* popt) {
	popt->options = OPTION_NONE;
	popt->target_file_no = 0;

  int double_hyphen = FALSE;
	int i;
	// parse comand line
	for(i = 1; i < argc; i++){
		printf_debug("[%d]: %s\n", i, argv[i]);
		if (!double_hyphen && strncmp(argv[i], "-", 1) == 0) {
			if (strncmp(argv[i] + 1, "-", 1) == 0) {
				// long option
				if (strcmp(argv[i] + 2, "filter") == 0) {
					popt->options |= OPTION_FILTER;
				}
				if (strcmp(argv[i] + 2, "help") == 0) {
					popt->options = OPTION_HELP;
					return 0;
				}
				if (strcmp(argv[i] + 2, "") == 0) {
					double_hyphen = TRUE;
					printf_debug("detect double-pyphen\n");
				}
			} else {
				// short option
				if (strcmp(argv[i] + 1, "f") == 0) {
					popt->options |= OPTION_FILTER;
				}
				if (strcmp(argv[i] + 1, "h") == 0) {
					popt->options = OPTION_HELP;
					return 0;
				}
			}
			if (!double_hyphen && popt->options == OPTION_NONE) {
				popt->options = OPTION_HELP;
				return 0;
			}
		} else {
			printf_debug("target=[%s]\n", argv[i]);
			if (popt->target_file_no == 0) {
				popt->target_file_no = i;
			}
		}
	}
	return 0;
}
