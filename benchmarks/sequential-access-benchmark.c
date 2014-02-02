#include "results.h"
#include "shared.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>

#define MEASUREMENT_FREQUENCY_IN_PAGE_ACCESSES (16384)


typedef struct {
    int (*function)(void *mem, size_t length, int num_passes, advice_t advice, char *desc);
    char *desc;
} function_t;


advice_t advice[] = {
    {POSIX_MADV_NORMAL, "POSIX_MADV_NORMAL"},
    {POSIX_MADV_RANDOM, "POSIX_MADV_RANDOM"},
    {POSIX_MADV_SEQUENTIAL, "POSIX_MADV_SEQUENTIAL"},
    {POSIX_MADV_NORMAL | POSIX_MADV_WILLNEED, "POSIX_MADV_NORMAL | POSIX_MADV_WILLNEED"},
    {POSIX_MADV_RANDOM | POSIX_MADV_WILLNEED, "POSIX_MADV_RANDOM | POSIX_MADV_WILLNEED"},
    {POSIX_MADV_SEQUENTIAL | POSIX_MADV_WILLNEED, "POSIX_MADV_SEQUENTIAL | POSIX_MADV_WILLNEED"},
};

static int seq_writes_benchmark(void *mem, size_t length, int num_passes, advice_t advice, char *desc);
static int seq_reads_benchmark(void *mem, size_t length, int num_passes, advice_t advice, char *desc);
static function_t functions[] = {
    {seq_writes_benchmark, "seq_writes"},
    {seq_reads_benchmark, "seq_reads"},
};


static int seq_writes_benchmark(void *mem, size_t length, int num_passes, advice_t advice, char *desc) {
    long page_size = sysconf(_SC_PAGE_SIZE);
    char buf[page_size];

    struct timeval start, end;
    size_t next_log = MEASUREMENT_FREQUENCY_IN_PAGE_ACCESSES;
    size_t page_accesses = length / page_size * num_passes;
    size_t bytes_copied = 0;
    size_t pages_accessed = 0;
    char *current_page = mem;

    results_t *results;
    if(results_init(&results, MEASUREMENT_FREQUENCY_IN_PAGE_ACCESSES, length, num_passes, 1, advice.desc, desc) != 0) {
        return 1;
    }

    gettimeofday(&start, NULL);
    for(size_t i = 0; i < page_accesses; i++) {
        memcpy(buf, current_page, page_size);
        bytes_copied += page_size;
        pages_accessed++;

        current_page += page_size;
        if(current_page >= (char *)mem + length) {
            // Loop back to the beginning
            current_page = mem;
        }

        // log performance periodically
        if(i+1 == next_log) {
            gettimeofday(&end, NULL);

            results_log(results, &start, &end, bytes_copied, pages_accessed);
            bytes_copied = 0;
            pages_accessed = 0;
            next_log += MEASUREMENT_FREQUENCY_IN_PAGE_ACCESSES;

            gettimeofday(&start, NULL);
        }
    }
    gettimeofday(&end, NULL);

    if(bytes_copied != 0) {
        results_log(results, &start, &end, bytes_copied, pages_accessed);
    }

    int ret = results_finished(results);
    results_destroy(results);
    return ret;
}


static int seq_reads_benchmark(void *mem, size_t length, int num_passes, advice_t advice, char *desc) {
    long page_size = sysconf(_SC_PAGE_SIZE);
    char buf[page_size];
    memset(buf, 0, page_size);

    struct timeval start, end;
    size_t next_log = MEASUREMENT_FREQUENCY_IN_PAGE_ACCESSES;
    size_t page_accesses = length / page_size * num_passes;
    size_t bytes_copied = 0;
    size_t pages_accessed = 0;
    char *current_page = mem;

    results_t *results;
    if(results_init(&results, MEASUREMENT_FREQUENCY_IN_PAGE_ACCESSES, length, num_passes, 1, advice.desc, desc) != 0) {
        return 1;
    }

    gettimeofday(&start, NULL);
    for(size_t i = 0; i < page_accesses; i++) {
        memcpy(current_page, buf, page_size);
        bytes_copied += page_size;
        pages_accessed++;

        current_page += page_size;
        if(current_page >= (char *)mem + length) {
            // Loop back to the beginning
            current_page = mem;
        }

        // log performance periodically
        if(i+1 == next_log) {
            gettimeofday(&end, NULL);

            results_log(results, &start, &end, bytes_copied, pages_accessed);
            bytes_copied = 0;
            pages_accessed = 0;
            next_log += MEASUREMENT_FREQUENCY_IN_PAGE_ACCESSES;

            gettimeofday(&start, NULL);
        }
    }
    gettimeofday(&end, NULL);

    if(bytes_copied != 0) {
        results_log(results, &start, &end, bytes_copied, pages_accessed);
    }

    int ret = results_finished(results);
    results_destroy(results);
    return ret;
}


static int benchmark0(char *file_name, size_t file_size, int num_passes, advice_t advice, function_t function) {
    fprintf(stderr, "Running sequential access benchmark: file_size=%ld, num_passes=%d, test=%s\n", file_size, num_passes, function.desc);

    int fd = create_uncached_initialized_file(file_name, file_size);
    if(fd == -1) {
        return 1;
    }

    void *mem = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(mem == MAP_FAILED) {
        perror("problem mmapping file");
        close(fd);
        return 1;
    }

    if(posix_madvise(mem, file_size, advice.advice) != 0) {
        perror("madvice() failed");
        return 1;
    }

    if(function.function(mem, file_size, num_passes, advice, function.desc) != 0) {
        munmap(mem, file_size);
        close(fd);
        return 1;
    }

    munmap(mem, file_size);
    close(fd);
    return 0;
}


static int benchmark(char *file_name, size_t file_size, int num_passes) {
    for(unsigned int i = 0; i < sizeof(functions)/sizeof(functions[0]); i++) {
        for(unsigned int j = 0; j < sizeof(advice)/sizeof(advice[0]); j++) {
            if(benchmark0(file_name, file_size, num_passes, advice[j], functions[i]) != 0) {
                return 1;
            }
        }
    }
    return 0;
}


int main(int argc, char **argv) {
    if(argc != 3) {
        fprintf(stderr, "usage: %s file_size num_passes\n", argv[0]);
        return EXIT_FAILURE;
    }

    size_t file_size;
    if(parse_human_readable_size(argv[1], &file_size) != 0) {
        return EXIT_FAILURE;
    }

    long page_size = sysconf(_SC_PAGE_SIZE);
    if(file_size % page_size != 0) {
        fprintf(stderr, "file_size must be a multiple of page size (%ld)\n", page_size);
        return EXIT_FAILURE;
    }

    int num_passes;
    if(parse_int(argv[2], &num_passes) != 0) {
        return EXIT_FAILURE;
    }
    if(num_passes < 1) {
        fprintf(stderr, "num_passes must be a positive integer\n");
        return EXIT_FAILURE;
    }

    fprintf(stderr, "page size:   %ld\n", page_size);
    fprintf(stderr, "file size:   %ld\n", file_size);
    fprintf(stderr, "num passes:  %d\n", num_passes);
    fprintf(stderr, "\n");

    if(benchmark("sequential-access.bin", file_size, num_passes) != 0) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
