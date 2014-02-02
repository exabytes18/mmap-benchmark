#include "shared.h"

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


int create_uncached_initialized_file(const char *path, size_t length) {
    int flags = O_WRONLY | O_CREAT | O_TRUNC;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

#ifdef __APPLE__
    int fd = open(path, flags, mode);
    if(fd == -1) {
        perror("problem opening file");
        return -1;
    }

    if(fcntl(fd, F_NOCACHE, 1) == -1) {
        perror("problem setting fd as F_NOCACHE");
        close(fd);
        return -1;
    }
#else
    int fd = open(path, flags | O_DIRECT, mode);
    if(fd == -1) {
        perror("problem opening file");
        return -1;
    }
#endif

    long page_size = sysconf(_SC_PAGE_SIZE);
    size_t buf_size = length >= 1024*1024 ? 1024*1024 : page_size;

    void *buf;
    if(posix_memalign(&buf, page_size, buf_size) != 0) {
        perror("failed to allocate buffer");
        return -1;
    }

    for(size_t i = 0; i < buf_size; i++) {
        ((char *)buf)[i] = (char)i;
    }

    size_t bytes_remaining = length;
    while(bytes_remaining > 0) {
        size_t bytes_to_write = bytes_remaining > buf_size ? buf_size : bytes_remaining;
        ssize_t bytes_written = write(fd, buf, bytes_to_write);
        if(bytes_written == -1) {
            perror("problem writing bytes to file");
            close(fd);
            free(buf);
            return -1;
        }
        bytes_remaining -= bytes_written;
    }

    close(fd);
    free(buf);

    fd = open(path, O_RDWR, mode);
    if(fd == -1) {
        perror("problem opening file");
        return -1;
    }

    return fd;
}


static int parse_multiplier(const char *arg, size_t *result) {
    const char *str = arg;
    while(str != '\0' && isspace(*str)) {
        str++;
    }

    if(strcasecmp(str, "") == 0 || strcasecmp(str, "b") == 0) {
        *result = 1ULL;
        return 0;
    } else if(strcasecmp(str, "k") == 0 || strcasecmp(str, "kb") == 0) {
        *result = 1ULL << 10;
        return 0;
    } else if(strcasecmp(str, "m") == 0 || strcasecmp(str, "mb") == 0) {
        *result = 1ULL << 20;
        return 0;
    } else if(strcasecmp(str, "g") == 0 || strcasecmp(str, "gb") == 0) {
        *result = 1ULL << 30;
        return 0;
    } else if(strcasecmp(str, "t") == 0 || strcasecmp(str, "tb") == 0) {
        *result = 1ULL << 40;
        return 0;
    } else if(strcasecmp(str, "p") == 0 || strcasecmp(str, "pb") == 0) {
        *result = 1ULL << 40;
        return 0;
    } else if(strcasecmp(str, "e") == 0 || strcasecmp(str, "eb") == 0) {
        *result = 1ULL << 50;
        return 0;
    } else if(strcasecmp(str, "z") == 0 || strcasecmp(str, "zb") == 0) {
        fprintf(stderr, "zettabytes, really?\n");
        return 1;
    } else if(strcasecmp(str, "y") == 0 || strcasecmp(str, "yb") == 0) {
        fprintf(stderr, "yottabytes, impossible.\n");
        return 1;
    } else {
        fprintf(stderr, "unknown unit\n");
        return 1;
    }
}


int parse_int(const char *arg, int *result) {
    char *ptr = NULL;
    errno = 0;

    int x = strtol(arg, &ptr, 10);
    if(errno != 0 || arg == ptr) {
        fprintf(stderr, "problem parsing string as integer: %s\n", arg);
        return 1;
    }

    *result = x;
    return 0;
}


int parse_human_readable_size(const char *arg, size_t *result) {
    char *ptr = NULL;
    errno = 0;

    ssize_t first_num = strtoll(arg, &ptr, 10);
    if(errno != 0 || arg == ptr) {
        fprintf(stderr, "problem parsing argument: %s\n", arg);
        return 1;
    }

    size_t multiplier;
    if(parse_multiplier(ptr, &multiplier) != 0) {
        fprintf(stderr, "problem parsing argument: %s\n", arg);
        return 1;
    }

    // We /could/ overflow here, but it really isn't worth checking.
    ssize_t s = first_num * multiplier;
    if(s <= 0) {
        fprintf(stderr, "invalid argument: %s\n", arg);
        return 1;
    }

    *result = s;
    return 0;
}
