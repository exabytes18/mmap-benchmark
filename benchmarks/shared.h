#ifndef __SHARED_H
#define __SHARED_H

#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE 1
#define _DARWIN_C_SOURCE 1

#include <stddef.h>
#include <sys/time.h>

typedef struct {
    int advice;
    char *desc;
} advice_t;

void **random_page_addresses(void *mem, size_t length);
void log_measurement(size_t test_file_size, size_t test_num_page_accesses, int num_threads, char *mmap_options, char *desc, struct timeval *start, struct timeval *end, size_t bytes, size_t total_bytes_copied, double percent_complete);
int create_uncached_initialized_file(const char *path, size_t length);
int parse_int(const char *arg, int *result);
int parse_human_readable_size(const char *arg, size_t *result);

#endif
