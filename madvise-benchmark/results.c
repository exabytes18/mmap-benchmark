#include "shared.h"
#include "results.h"

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>


struct _results_t {
    size_t print_frequency_in_page_accesses;
    size_t expected_num_page_accesses;

    size_t test_file_size;
    int num_passes;
    int num_threads;
    char *mmap_advice_desc;
    char *desc;
    pthread_mutex_t mutex;

    size_t total_bytes_copied;
    size_t total_pages_accessed;
    size_t bytes_copied_since_last_print;
    size_t pages_accessed_since_last_print;

    struct timeval prev;
    struct timeval cur;
};


static void results_print(struct _results_t *results) {
    time_t elapsed_time_us = results->cur.tv_usec - results->prev.tv_usec;
    elapsed_time_us += 1000000l * (results->cur.tv_sec - results->prev.tv_sec);

    double time_in_seconds = elapsed_time_us / 1e6;
    double pages_per_second = results->pages_accessed_since_last_print / time_in_seconds;
    double bytes_per_second = results->bytes_copied_since_last_print / time_in_seconds;
    double percent_complete = (double)results->total_pages_accessed / (double)results->expected_num_page_accesses * 100;

    // Use printf here only; all other output should go to stderr.
    printf("%ld,%d,%d,%s,%s,%ld,%ld,%lf,%lf\n",
           results->test_file_size,
           results->num_passes,
           results->num_threads,
           results->mmap_advice_desc,
           results->desc,
           results->total_bytes_copied,
           results->bytes_copied_since_last_print,
           time_in_seconds,
           bytes_per_second);

    // Dump some more human friendly metrics to stderr
    fprintf(stderr, "  %5.1lf complete (%9.3lf MBps, %10.1lf pages/sec)\n",
            percent_complete,
            bytes_per_second / 1024 / 1024,
            pages_per_second);

    results->pages_accessed_since_last_print = 0;
    results->bytes_copied_since_last_print = 0;
}


int results_init(struct _results_t **results, size_t print_frequency_in_page_accesses, size_t test_file_size, int num_passes, int num_threads, char *mmap_advice_desc, char *desc) {
    *results = malloc(sizeof((*results)[0]));
    if(*results == NULL) {
        fprintf(stderr, "Failed to malloc results struct\n");
        return 1;
    }

    if(pthread_mutex_init(&((*results)->mutex), NULL) != 0) {
        perror("Failed to initialize mutex");
        free(*results);
    }

    long page_size = sysconf(_SC_PAGE_SIZE);
    size_t num_pages = test_file_size / page_size;

    (*results)->print_frequency_in_page_accesses = print_frequency_in_page_accesses;
    (*results)->expected_num_page_accesses = num_pages * num_passes;

    (*results)->test_file_size = test_file_size;
    (*results)->num_passes = num_passes;
    (*results)->num_threads = num_threads;
    (*results)->mmap_advice_desc = mmap_advice_desc;
    (*results)->desc = desc;

    (*results)->total_bytes_copied = 0;
    (*results)->total_pages_accessed = 0;
    (*results)->bytes_copied_since_last_print = 0;
    (*results)->pages_accessed_since_last_print = 0;

    gettimeofday(&(*results)->prev, NULL);
    gettimeofday(&(*results)->cur, NULL);

    return 0;
}


void results_log(struct _results_t *results, size_t bytes_copied, size_t num_pages_accessed) {
    pthread_mutex_lock(&(results->mutex));

    results->total_bytes_copied += bytes_copied;
    results->total_pages_accessed += num_pages_accessed;
    results->bytes_copied_since_last_print += bytes_copied;
    results->pages_accessed_since_last_print += num_pages_accessed;

    if(results->pages_accessed_since_last_print >= results->print_frequency_in_page_accesses) {
        gettimeofday(&results->cur, NULL);
        results_print(results);
        results->prev = results->cur;
    }

    pthread_mutex_unlock(&(results->mutex));
}


int results_finished(struct _results_t *results) {
    if(results->pages_accessed_since_last_print > 0) {
        gettimeofday(&results->cur, NULL);
        results_print(results);
        results->prev = results->cur;
    }

    if(results->total_pages_accessed != results->expected_num_page_accesses) {
        fprintf(stderr, "Accessed %ld pages; expected %ld accesses\n",
                results->total_pages_accessed,
                results->expected_num_page_accesses);
        return 1;
    }
    return 0;
}


int results_destroy(struct _results_t *results) {
    if(pthread_mutex_destroy(&(results->mutex)) != 0) {
        perror("Failed to destroy mutex");
        return 1;
    }

    free(results);
    return 0;
}
