#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h> /* for stat */
#include <unistd.h> /* close */
#include <fcntl.h> /* for open(2) */
#include <sys/mman.h> /* mmap/munmap */

#include "csvparse.h"
#include "debugger.h"
#include "command_option.h"

void print_usage(const char const* self) {
	printf("Usage: %s [options] file [file...]\n", self);
	printf("    -f, --filter                     filter mode\n");
	printf("    -h, --help                       print this message\n");
}

int get_file_size(const char* filename, size_t* filesize) {
	struct stat filestat;
	if (stat(filename, &filestat) == -1) {
		return -1;
	}
	printf_debug("filesize=%ld\n", filestat.st_size);
	*filesize = filestat.st_size;
	return filestat.st_size;
}
void parse_csv(const char* filename) {
	// open file fd
	int fd;
	if ((fd = open(filename, O_RDONLY)) == -1) {
		perror(filename);
		return;
	}
	// retrieve filesize
	size_t filesize;
	if ((filesize = get_file_size(filename, &filesize)) == -1) {
		perror(filename);
		return;
	}
	
	// attach payload
	char* p_sof = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
	//printf_string_debug(0, p_sof);

	int quote_in = FALSE;
	size_t it;
	char* p = p_sof;
	for (it = 0; it < filesize; it++, p++) {
		if (*p == ',' || *p == LF_C || *p == CR_C || (*p == CR_C && *(p+1) == LF_C)) {
			if (quote_in) {
				// そのまま
				if (*p == CR_C && *(p+1) == LF_C) {
					printf_string_debug(it, "CRLF", "");
					it++;p++;
				} else {
					printf_char_debug(it, *p, "");
				}
			} else {
				if (*p == CR_C && *(p+1) == LF_C) {
					// EOL
					printf_string_debug(it, "EOL", "");
					it++;p++;
				} else if (*p == ',') {
					// delimiter
					printf_char_debug(it, *p, "delimiter");
				} else {
    			printf_char_debug(it, *p, ""); // そのまま
				}
			}
		} else if (*p == QUOTE_C) {
			if (quote_in) {
				if (*(p+1) == QUOTE_C) { // 重ね？
					printf_char_debug(it, QUOTE_C, "escaped-quote");
					it++;p++;
				} else {
					quote_in = FALSE;
    			printf_char_debug(it, *p, "quote out"); // そのまま
				}
			} else {
				quote_in = TRUE;
  			printf_char_debug(it, *p, "quote in"); // そのまま
			}
		} else {
 			printf_char_debug(it, *p, ""); // そのまま
		}
	}

	// cleanup
	munmap(p_sof, filesize);
	close(fd);
	return;
}

int main(int argc, char *argv[]) {
	// parse command-line option
	COMMAND_OPTION opt;
	if (parse_command_option(argc, argv, &opt) == -1) {
		perror("command parser error.");
		return 1;
	}
	if (opt.options == OPTION_HELP) {
		print_usage(argv[0]);
		return 0;
	}

	// main procedure
	int data_from_stdin = FALSE;
	if (opt.target_file_no == 0) {
		// get payload from stdin
		data_from_stdin = TRUE;
		opt.target_file_no = argc - 1;
	}
	int i;
	for (i = opt.target_file_no; i < argc; i++) {
		if (data_from_stdin) {
			//parse_csv(&stdin);
		} else {
			parse_csv(argv[i]);
		}
	}
	return 0;
}
