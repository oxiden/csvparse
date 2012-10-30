#include <string.h> /* for strcmp */
#include "csvparse.h"

typedef struct {
	int options;
	int target_file_no;
} COMMAND_OPTION;

int parse_command_option(int argc, char* argv[], COMMAND_OPTION* popt);
