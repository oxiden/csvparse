#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h> /* for stat */
#include <fcntl.h> /* for open */
#include <unistd.h> /* for close */
#include <sys/mman.h> /* for mmap/munmap */

#include "csvparse.h"
#include "debugger.h"
#include "command_option.h"

void print_usage(const char const* self) {
	printf("Usage: %s [options] file [file...]\n", self);
	printf("    -f, --filter   filter mode\n");
	printf("    -h, --help     print this message\n");
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
void parse_csv(const char* filename, int opts) {
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
					if (opts & OPTION_FILTER) {
						printf("${CRLF}");
					} else {
						printf("%c%c", CR_C, LF_C);
					}
					it++;p++;
				} else if (*p == LF_C) {
					printf_char_debug(it, LF_C, "");
					if (opts & OPTION_FILTER) {
						printf("${LF}");
					} else {
						printf("%c", LF_C);
					}
				} else if (*p == CR_C) {
					printf_char_debug(it, CR_C, "");
					if (opts & OPTION_FILTER) {
						printf("${CR}");
					} else {
						printf("%c", CR_C);
					}
				} else {
					printf_char_debug(it, *p, "");
					printf("%c", *p);
				}
			} else {
				if (*p == CR_C && *(p+1) == LF_C) {
					printf_string_debug(it, "EOL", ""); // EOL
					printf("%c%c", CR_C, LF_C);
					it++;p++;
				} else if (*p == ',') {
					printf_char_debug(it, *p, "delimiter"); // delimiter
					printf("%c", *p);
				} else {
    			printf_char_debug(it, *p, ""); // そのまま
					printf("%c", *p);
				}
			}
		} else if (*p == QUOTE_C) {
			if (quote_in) {
				if (*(p+1) == QUOTE_C) { // 重ね？
					printf_char_debug(it, QUOTE_C, "escaped-quote");
					printf("%c", QUOTE_C);
					it++;p++;
				} else {
					quote_in = FALSE;
          printf_char_debug(it, QUOTE_C, "quote out"); // そのまま
					printf("%c", QUOTE_C);
				}
			} else {
				quote_in = TRUE;
        printf_char_debug(it, QUOTE_C, "quote in"); // そのまま
					printf("%c", QUOTE_C);
			}
		} else {
 			printf_char_debug(it, *p, ""); // そのまま
			printf("%c", *p);
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
			parse_csv(argv[i], opt.options);
		}
	}
	return 0;
}
