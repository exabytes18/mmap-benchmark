#ifndef __BQ_H
#define __BQ_H

#include <stddef.h>

typedef struct _bq_t bq_t;

/**
 * Initializes the bq struct. Space for all elements is allocated up front.
 */
int bq_init(bq_t **bq, size_t size);
/**
 * Destroys the bq struct
 */
int bq_destroy(bq_t *bq);
/**
 * Indicates that no more elements will be enqueued. Once all elements have
 * been dequeued, bq_dequeue calls will return immediately and not block.
 */
void bq_finished(bq_t *bq);
/**
 * Enqueues an element in the bq specified. Blocks if no space is available.
 */
int bq_enqueue(bq_t *bq, void *ptr);
/**
 * Dequeues an element from the bq specified. Returns 0 and sets ptr if an
 * element was dequeued; returns 1 if bq_finished has been called on this
 * bq. This call may block.
 */
int bq_dequeue(bq_t *bq, void **ptr);

#endif
