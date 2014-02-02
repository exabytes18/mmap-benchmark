#include "shared.h"
#include "bq.h"
#include "results.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MEASUREMENT_FREQUENCY_IN_PAGE_ACCESSES (16384)
#define PAGES_PER_CHUNK (512)
#define CHUNK_QUEUE_SIZE (1024)


typedef struct {
    int (*function)(void *mem, size_t length, int num_passes, int num_threads, advice_t advice, char *desc);
    char *desc;
} function_t;

typedef struct {
    void **page_indexes;
    size_t num_pages;
} chunk_t;

typedef struct {
    void **page_indexes;
    chunk_t *chunks;
    size_t num_chunks;
} chunks_t;

typedef struct {
    bq_t *bq;
    results_t *results;
} thread_params_t;


advice_t advice[] = {
    {POSIX_MADV_NORMAL, "POSIX_MADV_NORMAL"},
    {POSIX_MADV_RANDOM, "POSIX_MADV_RANDOM"},
    {POSIX_MADV_SEQUENTIAL, "POSIX_MADV_SEQUENTIAL"},
    {POSIX_MADV_NORMAL | POSIX_MADV_WILLNEED, "POSIX_MADV_NORMAL|POSIX_MADV_WILLNEED"},
    {POSIX_MADV_RANDOM | POSIX_MADV_WILLNEED, "POSIX_MADV_RANDOM|POSIX_MADV_WILLNEED"},
    {POSIX_MADV_SEQUENTIAL | POSIX_MADV_WILLNEED, "POSIX_MADV_SEQUENTIAL|POSIX_MADV_WILLNEED"},
};

static int random_writes_benchmark(void *mem, size_t length, int num_passes, int num_threads, advice_t advice, char *desc);
static int random_reads_benchmark(void *mem, size_t length, int num_passes, int num_threads, advice_t advice, char *desc);
static function_t functions[] = {
    {random_writes_benchmark, "random_writes"},
    {random_reads_benchmark, "random_reads"},
};


// From http://stackoverflow.com/a/2999130
static int rand_lim(int limit) {
    int divisor = RAND_MAX/(limit+1);
    int retval;

    do {
        retval = rand() / divisor;
    } while(retval > limit);

    return retval;
}


static void **random_page_addresses(void *mem, size_t length) {
    long page_size = sysconf(_SC_PAGE_SIZE);

    size_t num_pages = length / page_size;
    void **random_pages = malloc(num_pages * sizeof(random_pages[0]));
    if(random_pages == NULL) {
        return NULL;
    }

    for(size_t i = 0; i < num_pages; i++) {
        random_pages[i] = (char *)mem + i * page_size;
    }

    for(size_t i = 0; i < num_pages; i++) {
        size_t r = i + rand_lim(num_pages - i - 1);
        if(i != r) {
            void *t = random_pages[i];
            random_pages[i] = random_pages[r];
            random_pages[r] = t;
        }
    }

    return random_pages;
}


static int chunkify(void *mem, size_t length, chunks_t **chunks) {
    long page_size = sysconf(_SC_PAGE_SIZE);
    size_t num_pages = length / page_size;

    *chunks = malloc(sizeof((*chunks)[0]));
    if(*chunks == NULL) {
        fprintf(stderr, "Failed to malloc chunks struct\n");
        return 1;
    }

    (*chunks)->num_chunks = (num_pages / PAGES_PER_CHUNK) + (num_pages % PAGES_PER_CHUNK == 0 ? 0 : 1);
    (*chunks)->chunks = malloc(sizeof(((*chunks)->chunks)[0]) * ((*chunks)->num_chunks));
    if((*chunks)->chunks == NULL) {
        fprintf(stderr, "Failed to malloc chunks array\n");
        free(*chunks);
        return 1;
    }

    (*chunks)->page_indexes = random_page_addresses(mem, length);
    if((*chunks)->page_indexes == NULL) {
        fprintf(stderr, "Failed to malloc random pages array\n");
        free((*chunks)->chunks);
        free(*chunks);
        return 1;
    }

    size_t pages_remaining = num_pages;
    void **page_indexes = (*chunks)->page_indexes;
    for(size_t i = 0; i < (*chunks)->num_chunks; i++) {
        chunk_t *chunk = (*chunks)->chunks + i;
        chunk->page_indexes = page_indexes;
        chunk->num_pages = MIN(pages_remaining, PAGES_PER_CHUNK);
        page_indexes += chunk->num_pages;
        pages_remaining -= chunk->num_pages;
    }
    return 0;
}


static void free_chunks(chunks_t *chunks) {
    free(chunks->page_indexes);
    free(chunks->chunks);
    free(chunks);
}


static int enqueue_chunks(bq_t *bq, chunks_t *chunks) {
    for(size_t i = 0; i < chunks->num_chunks; i++) {
        if(bq_enqueue(bq, chunks->chunks + i) != 0) {
            fprintf(stderr, "Problem enqueing chunk\n");
            return 1;
        }
    }
    return 0;
}


static void *random_writes_thread(void *ptr) {
    thread_params_t *params = ptr;
    bq_t *bq = params->bq;
    results_t *results = params->results;

    long page_size = sysconf(_SC_PAGE_SIZE);
    char buf[page_size];
    memset(buf, 0, page_size);

    chunk_t *chunk;
    while(bq_dequeue(bq, (void **)&chunk) == 0) {
        struct timeval start, end;
        size_t bytes_copied = 0;

        gettimeofday(&start, NULL);
        for(size_t i = 0; i < chunk->num_pages; i++) {
            memcpy(chunk->page_indexes[i], buf, page_size);
            bytes_copied += page_size;
        }
        gettimeofday(&end, NULL);

        results_log(results, &start, &end, bytes_copied, chunk->num_pages);
    }

    return NULL;
}


static void *random_reads_thread(void *ptr) {
    thread_params_t *params = ptr;
    bq_t *bq = params->bq;
    results_t *results = params->results;

    long page_size = sysconf(_SC_PAGE_SIZE);
    char buf[page_size];

    chunk_t *chunk;
    while(bq_dequeue(bq, (void **)&chunk) == 0) {
        struct timeval start, end;
        size_t bytes_copied = 0;

        gettimeofday(&start, NULL);
        for(size_t i = 0; i < chunk->num_pages; i++) {
            memcpy(buf, chunk->page_indexes[i], page_size);
            bytes_copied += page_size;
        }
        gettimeofday(&end, NULL);

        results_log(results, &start, &end, bytes_copied, chunk->num_pages);
    }

    return NULL;
}


static int run_threaded_benchmark(void *mem, size_t length, int num_passes, int num_threads, advice_t advice, char *desc, void *(thread_function)(void *)) {
    bq_t *bq;
    if(bq_init(&bq, CHUNK_QUEUE_SIZE) != 0) {
        return 1;
    }

    chunks_t *chunks;
    if(chunkify(mem, length, &chunks) != 0) {
        bq_destroy(bq);
        return 1;
    }

    results_t *results;
    if(results_init(&results, MEASUREMENT_FREQUENCY_IN_PAGE_ACCESSES, length, num_passes, num_threads, advice.desc, desc) != 0) {
        bq_destroy(bq);
        free_chunks(chunks);
        return 1;
    }

    // Setup the thread params
    thread_params_t *thread_params = malloc(sizeof(thread_params[0]) * num_threads);
    if(thread_params == NULL) {
        bq_destroy(bq);
        free_chunks(chunks);
        results_destroy(results);
        return 1;
    }
    for(int i = 0; i < num_threads; i++) {
        thread_params[i].bq = bq;
        thread_params[i].results = results;
    }

    // Initialize and start our threads
    pthread_t threads[num_threads];
    for(int i = 0; i < num_threads; i++) {
        if(pthread_create(&threads[i], NULL, thread_function, &thread_params[i]) != 0) {
            perror("Problem creating thread");
            bq_finished(bq);
            for(int j = 0; j < i; j++) {
                if(pthread_join(threads[j], NULL) != 0) {
                    perror("Problem joining thread");
                }
            }
            bq_destroy(bq);
            free_chunks(chunks);
            results_destroy(results);
            free(thread_params);
            return 1;
        }
    }

    // Start enqueuing the chunks
    int ret = 0;
    for(int pass = 0; pass < num_passes; pass++) {
        if(enqueue_chunks(bq, chunks) != 0) {
            ret = 1;
            break;
        }
    }
    bq_finished(bq);

    // await thread completion
    for(int i = 0; i < num_threads; i++) {
        if(pthread_join(threads[i], NULL) != 0) {
            perror("Problem joining thread");
        }
    }

    if(results_finished(results) != 0) {
        ret = 1;
    }

    bq_destroy(bq);
    free_chunks(chunks);
    results_destroy(results);
    free(thread_params);
    return ret;
}


static int random_writes_benchmark(void *mem, size_t length, int num_passes, int num_threads, advice_t advice, char *desc) {
    return run_threaded_benchmark(mem, length, num_passes, num_threads, advice, desc, random_writes_thread);
}


static int random_reads_benchmark(void *mem, size_t length, int num_passes, int num_threads, advice_t advice, char *desc) {
    return run_threaded_benchmark(mem, length, num_passes, num_threads, advice, desc, random_reads_thread);
}


static int benchmark0(char *file_name, size_t file_size, int num_passes, int num_threads, advice_t advice, function_t function) {
    fprintf(stderr, "Running random access benchmark: file_size=%ld, num_passes=%d, num_threads=%d, test=%s, advice=%s\n",
            file_size,
            num_passes,
            num_threads,
            function.desc,
            advice.desc);

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
