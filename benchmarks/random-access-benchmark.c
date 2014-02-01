#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>
#include "shared.h"

#define MEASUREMENT_FREQUENCY_IN_PAGE_ACCESSES (16384)


typedef struct {
    int (*function)(void *mem, size_t length, int num_passes, int num_threads, advice_t advice, char *desc);
    char *desc;
} function_t;


static int random_writes_benchmark(void *mem, size_t length, int num_passes, int num_threads, advice_t advice, char *desc);
static int random_reads_benchmark(void *mem, size_t length, int num_passes, int num_threads, advice_t advice, char *desc);
static function_t functions[] = {
    {random_writes_benchmark, "random_writes"},
    {random_reads_benchmark, "random_reads"},
};


static int random_writes_benchmark(void *mem, size_t length, int num_passes, int num_threads, advice_t advice, char *desc) {
    // TODO: implement
    return 0;
}


static int random_reads_benchmark(void *mem, size_t length, int num_passes, int num_threads, advice_t advice, char *desc) {
    // TODO: implement
    return 0;
}


static int benchmark0(char *file_name, size_t file_size, int num_passes, int num_threads, advice_t advice, function_t function) {
    fprintf(stderr, "Running random access benchmark: file_size=%ld, num_passes=%d, num_threads=%d, test=%s\n", file_size, num_passes, num_threads, function.desc);

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

    if(madvise(mem, file_size, advice.advice) != 0) {
        perror("madvice() failed");
        return 1;
    }

    if(function.function(mem, file_size, num_passes, num_threads, advice, function.desc) != 0) {
        munmap(mem, file_size);
        close(fd);
        return 1;
    }

    munmap(mem, file_size);
    close(fd);
    return 0;
}


static int benchmark(char *file_name, size_t file_size, int num_passes, int num_threads) {
    for(unsigned int i = 0; i < sizeof(functions)/sizeof(functions[0]); i++) {
        for(unsigned int j = 0; j < sizeof(advice)/sizeof(advice[0]); j++) {
            if(benchmark0(file_name, file_size, num_passes, num_threads, advice[j], functions[i]) != 0) {
                return 1;
            }
        }
    }
    return 0;
}


int main(int argc, char **argv) {
    if(argc != 4) {
        fprintf(stderr, "usage: %s file_size num_passes num_threads\n", argv[0]);
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

    int num_threads;
    if(parse_int(argv[3], &num_threads) != 0) {
        return EXIT_FAILURE;
    }
    if(num_threads < 1) {
        fprintf(stderr, "num_threads must be a positive integer\n");
        return EXIT_FAILURE;
    }

    fprintf(stderr, "page size:   %ld\n", page_size);
    fprintf(stderr, "file size:   %ld\n", file_size);
    fprintf(stderr, "num passes:  %d\n", num_passes);
    fprintf(stderr, "num threads: %d\n", num_threads);
    fprintf(stderr, "\n");

    if(benchmark("random-access.bin", file_size, num_passes, num_threads) != 0) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
