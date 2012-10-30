#include <stdio.h> // for size_t

void printf_error(void);
void printf_debug(const char* format, ...);
void printf_string_debug(const size_t it, const char* str, const char* addtional_str);
void printf_char_debug(const size_t it, const char c, const char* addtional_str);
