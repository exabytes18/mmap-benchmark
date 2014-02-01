#ifdef __linux
#define _GNU_SOURCE
#endif

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// Make sure you have at least 16M of hugepages available.

static int create_uncached_initialized_file(const char *path, int additional_flags, size_t length) {
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
            return -1;
        }
        bytes_remaining -= bytes_written;
    }

    close(fd);
    free(buf);

    fd = open(path, O_RDWR | additional_flags, mode);
    if(fd == -1) {
        perror("problem opening file");
        return -1;
    }

    return fd;
}

int mmap_test(size_t length, int prot, int flags, int fd, off_t offset) {
    void *mem = mmap(NULL, length, prot, flags, fd, offset);
    int success = mem != MAP_FAILED;
    if(success) {
        printf("address: %p\n", mem);
        for(int i = 0; i < 1024; i++) {
            ((char *)mem)[i] = 0;
        }
        munmap(mem, length);
    } else {
        perror("unable to perform mmap");
    }
    return success;
}

int main(int argc, char **argv) {
    size_t alloc_size = 16 * 1024 * 1024;


    printf("Attempting mmap (no file):\n");
    mmap_test(alloc_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    printf("\n");


    printf("Attempting mmap (no file, with hugetlb):\n");
#ifdef MAP_HUGETLB
    mmap_test(alloc_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);
#else
    printf("MAP_HUGETLB not available\n");
#endif
    printf("\n");


    printf("Attempting mmap (file, no hugetlb):\n");
    int fd = create_uncached_initialized_file("hugetlb.bin", 0, alloc_size);
    if(fd == -1) {
        printf("unable to create file\n");
    } else {
        mmap_test(alloc_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    }
    close(fd);
    printf("\n");


    printf("Attempting mmap (file, with hugetlb):\n");
#ifdef MAP_HUGETLB
    int fd2 = create_uncached_initialized_file("hugetlb.bin", 0, alloc_size);
    if(fd2 == -1) {
        printf("unable to create file\n");
    } else {
        mmap_test(alloc_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_HUGETLB, fd2, 0);
    }
    close(fd2);
#else
    printf("MAP_HUGETLB not available\n");
#endif
    printf("\n");


    return EXIT_SUCCESS;
}
