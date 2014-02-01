#ifndef __SHARED_H
#define __SHARED_H

#include <sys/mman.h>

typedef struct {
    int advice;
    char *desc;
} advice_t;

// Don't test MADV_DONTNEED since it's destructive on some operating systems.
advice_t advice[] = {
    {MADV_NORMAL, "MADV_NORMAL"},
    {MADV_RANDOM, "MADV_RANDOM"},
    {MADV_SEQUENTIAL, "MADV_SEQUENTIAL"},
    {MADV_NORMAL | MADV_WILLNEED, "MADV_NORMAL | MADV_WILLNEED"},
    {MADV_RANDOM | MADV_WILLNEED, "MADV_RANDOM | MADV_WILLNEED"},
    {MADV_SEQUENTIAL | MADV_WILLNEED, "MADV_SEQUENTIAL | MADV_WILLNEED"},
};

void **random_page_addresses(void *mem, size_t length);
void log_measurement(size_t test_file_size, size_t test_num_page_accesses, char *mmap_options, char *desc, struct timeval *start, struct timeval *end, size_t bytes, size_t total_bytes_copied, double percent_complete);
int create_uncached_initialized_file(const char *path, size_t length);
int parse_int(const char *arg, int *result);
int parse_multiplier(const char *arg, size_t *result);
int parse_human_readable_size(const char *arg, size_t *result);

#endif
