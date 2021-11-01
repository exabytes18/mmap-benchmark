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


int create_uncached_initialized_file(const char *path, size_t length);

int parse_int(const char *arg, int *result);

int parse_human_readable_size(const char *arg, size_t *result);

#endif
