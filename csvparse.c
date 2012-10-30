#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* for strcmp */
#include <sys/stat.h> /* for stat */
#include <unistd.h>
#include <fcntl.h> /* for open(2) */
#include <sys/mman.h>

#include "csvparse.h"
#include "debugger.h"

/* CRLF char? */
int is_eol(const char const* p) {
 if (*p == CR_C && *(p+1) == LF_C) {
			printf_debug("\tdetect EOL\n");
			return CRLF;
	}
 return NORMAL_CHAR;
}

void print_usage(const char const* self) {
	printf("Usage: %s [options] file [file...]\n", self);
	printf("    -l, --line                       count lines\n");
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
	int options = OPTION_NONE;
	int target_file_no = 0;
  int double_hyphen = FALSE;

	int data_from_stdin = FALSE;

	int i;
	// parse comand line
	for(i = 1; i < argc; i++){
		printf_debug("[%d]: %s\n", i, argv[i]);
		if (!double_hyphen && strncmp(argv[i], "-", 1) == 0) {
			if (strncmp(argv[i] + 1, "-", 1) == 0) {
				// long option
				if (strcmp(argv[i] + 2, "word") == 0) {
					options |= OPTION_WORD;
				}
				if (strcmp(argv[i] + 2, "column") == 0) {
					options |= OPTION_COLUMN;
				}
				if (strcmp(argv[i] + 2, "line") == 0) {
					options |= OPTION_LINE;
				}
				if (strcmp(argv[i] + 2, "help") == 0) {
					print_usage(argv[0]);
					exit(0);
				}
				if (strcmp(argv[i] + 2, "") == 0) {
					double_hyphen = TRUE;
					printf_debug("detect double-pyphen\n");
				}
			} else {
				// short option
				if (strcmp(argv[i] + 1, "w") == 0) {
					options |= OPTION_WORD;
				}
				if (strcmp(argv[i] + 1, "c") == 0) {
					options |= OPTION_COLUMN;
				}
				if (strcmp(argv[i] + 1, "l") == 0) {
					options |= OPTION_LINE;
				}
				if (strcmp(argv[i] + 1, "h") == 0) {
					print_usage(argv[0]);
					exit(0);
				}
			}
			if (!double_hyphen && options == OPTION_NONE) {
				print_usage(argv[0]);
				exit(0);
			}
		} else {
			printf_debug("target=[%s]\n", argv[i]);
			if (target_file_no == 0) {
				target_file_no = i;
			}
		}
	}

	// main procedure
	if (target_file_no == 0) {
		// get payload from stdin
		data_from_stdin = TRUE;
		target_file_no = argc - 1;
	}
	for (i = target_file_no; i < argc; i++) {
			if (data_from_stdin) {
				//result = parse_csv(&stdin);
			} else {
				//result = open_csv(argv[i]);
				parse_csv(argv[i]);
			}
	}
	return 0;
}
