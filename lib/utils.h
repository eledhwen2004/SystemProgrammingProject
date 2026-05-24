#ifndef UTILS_H
#define UTILS_H

#include <sys/types.h>

#define MAX_FILES 32
#define MAX_NAME 256
#define MAX_HEADER 10000
#define MAX_TOTAL_SIZE (200 * 1024 * 1024)

int is_text_file(const char *filename);
off_t get_file_size(const char *filename);
int ends_with(const char *str, const char *suffix);

#endif