#ifndef _QUEUE_H
#define _QUEUE_H
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#define _QUEUE_SHRINK_FACTOR 2
#define _QUEUE_SHRINK_THRESHOLD_FACTOR 4
#define _QUEUE_GROWTH_FACTOR 2

typedef struct {
	unsigned int start;
	unsigned int end;
	unsigned int queue_size;
	size_t data_size;
	void *data;
	bool is_empty;
} queue;

int queue_init(queue *q, void *data, size_t data_size, unsigned int queue_size);
int queue_enqueue(queue *q, const void *data);
int queue_dequeue(queue *q, void *data);
int queue_peek(queue *q, void **data, unsigned int index);

int queue_d_init(queue *q, size_t data_size, unsigned int queue_size);
int queue_d_enqueue(queue *q, const void *data);
int queue_d_dequeue(queue *q, void *data);

static int queue_d_resize(queue *q, unsigned int size);
static inline int queue_d_grow(queue *q, unsigned int size);
static inline int queue_d_shrink(queue *q, unsigned int size);

#endif
