#ifndef __results_H
#define __results_H

#include <stddef.h>
#include <sys/time.h>


typedef struct _results_t results_t;


int results_init(results_t **results, size_t print_frequency_in_page_accesses, size_t test_file_size, int num_passes, int num_threads, char *mmap_advice_desc, char *desc);

void results_log(results_t *results, struct timeval *start, struct timeval *end, size_t bytes_copied, size_t num_pages);

int results_finished(struct _results_t *results);

int results_destroy(results_t *results);

#endif
