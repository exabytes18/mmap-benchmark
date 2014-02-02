#include "shared.h"
#include "bq.h"

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>


struct _bq_t {
    size_t size;
    size_t num_elements;
    void **queue;
    void **head;
    void **tail;
    int finished;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};


int bq_init(struct _bq_t **bq, size_t size) {
    *bq = malloc(sizeof((*bq)[0]));
    if(*bq == NULL) {
        fprintf(stderr, "Failed to malloc bq struct\n");
        return 1;
    }

    (*bq)->queue = malloc(sizeof((*bq)->queue[0]) * size);
    if((*bq)->queue == NULL) {
        fprintf(stderr, "Failed to malloc bq internal array\n");
        free(*bq);
        return 1;
    }

    if(pthread_mutex_init(&((*bq)->mutex), NULL) != 0) {
        perror("Failed to initialize mutex");
        free((*bq)->queue);
        free(*bq);
    }

    if(pthread_cond_init(&((*bq)->cond), NULL) != 0) {
        perror("Failed to destroy cond variable");
        if(pthread_mutex_destroy(&((*bq)->mutex)) != 0) {
            perror("Failed to destroy mutex");
        }
        free((*bq)->queue);
        free(*bq);
    }

    (*bq)->size = size;
    (*bq)->num_elements = 0;
    (*bq)->head = (*bq)->queue;
    (*bq)->tail = (*bq)->head;
    (*bq)->finished = 0;
    return 0;
}


int bq_destroy(struct _bq_t *bq) {
    if(pthread_cond_destroy(&(bq->cond)) != 0) {
        perror("Failed to destroy cond variable");
        return 1;
    }

    if(pthread_mutex_destroy(&(bq->mutex)) != 0) {
        perror("Failed to destroy mutex");
        return 1;
    }

    free(bq->queue);
    free(bq);
    return 0;
}


void bq_finished(struct _bq_t *bq) {
    pthread_mutex_lock(&(bq->mutex));
    bq->finished = 1;
    pthread_cond_broadcast(&(bq->cond));
    pthread_mutex_unlock(&(bq->mutex));
}


int bq_enqueue(struct _bq_t *bq, void *ptr) {
    pthread_mutex_lock(&(bq->mutex));
    while(bq->num_elements >= bq->size) {
        pthread_cond_wait(&(bq->cond), &(bq->mutex));
    }
    if(bq->tail == bq->queue + bq->size) {
        bq->tail = bq->queue;
    }
    bq->tail[0] = ptr;
    bq->tail++;
    bq->num_elements++;
    pthread_cond_signal(&(bq->cond));
    pthread_mutex_unlock(&(bq->mutex));
    return 0;
}


int bq_dequeue(struct _bq_t *bq, void **ptr) {
    pthread_mutex_lock(&(bq->mutex));
    while(bq->num_elements <= 0) {
        if(bq->finished) {
            pthread_mutex_unlock(&(bq->mutex));
            return 1;
        }
        pthread_cond_wait(&(bq->cond), &(bq->mutex));
    }
    if(bq->head == bq->queue + bq->size) {
        bq->head = bq->queue;
    }
    *ptr = bq->head[0];
    bq->head++;
    bq->num_elements--;
    pthread_cond_signal(&(bq->cond));
    pthread_mutex_unlock(&(bq->mutex));
    return 0;
}
