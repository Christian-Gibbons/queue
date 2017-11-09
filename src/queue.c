#include "queue.h"

/**
 * Initiazlize a fixed-size queue
 *
 * @param q - pointer to the queue to initialize
 * @param data - pointer to an array of data held by the queue TODO: let the queue handle allocation?
 * @param data_size - Number of bytes an element of the queue uses
 * @param queue_size - Max number of elmeents in the queue
 *
 * @return - error status
 **/
int queue_init(queue *q, void *data, size_t data_size, unsigned int queue_size) {
	if (data == NULL || !(data_size && queue_size)) {
		errno = EINVAL;
		return -1;
	}
	q->queue_size = queue_size;
	q->end = 0;
	q->start = 0;
	q->data_size = data_size;
	q->data = data;
	q->is_empty = true;

	return 0;
}

/**
 * Adds entry to the end of the queue
 *
 * @param q - queue to be operated on
 * @param data - entry to be added to end of queue
 *
 * @return error status
 **/
int queue_enqueue(queue *q, const void *data) {
	if (data == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (q->is_empty || q->start != q->end) {
		memcpy(((char *)q->data) + (q->end * q->data_size), data, q->data_size); // Flawfinder: ignore
		q->end++;
		q->end %= q->queue_size;
		q->is_empty = false;
		return 0;
	}
	else {
		errno = ENOBUFS;
		return -1;
	}
}

/**
 * Returns a pointer to an entry from the queue
 *
 * @param q - queue to be operated on
 * @param data - returns pointer to the element to be returned
 * @param index - the element to be returned
 *
 * @return - error status
 **/
int queue_peek(queue *q, void **data, unsigned int index) {
	if (data == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (q->is_empty || (q->start < q->end ? (q->start + index) > q->end :
				(q->start + index) > (q->end + q->queue_size))) {
		errno = ENODATA;
		return -1;
	}

	*data = (char *)q->data + ((q->start + index) % q->queue_size);
	return 0;
}

/**
 * Removes first element of the queue
 *
 * @param q - queue to be operated on
 * @param data - returns the first element of the queue
 *
 * @return - error status
 **/
int queue_dequeue(queue *q, void *data) {
	if (q->is_empty) {
		errno = ENODATA;
		return -1;
	}

	if (data != NULL) {
		memcpy(data, ((char *)q->data) + (q->start * q->data_size), q->data_size); // Flawfinder: ignore
	}
	q->start++;
	q->start %= q->queue_size;
	if (q->start == q->end) {
		q->is_empty = true;
	}
	return 0;
}

/**
 * Initialize a dynamic queue
 *
 * @param q - pointer to the queue to initialize
 * @param data_size - Number of bytes an element of the queue uses
 * @param queue_size - initial number of elements to reserve space for in queue
 *
 * @return - error status
 **/
int queue_d_init(queue *q, size_t data_size, unsigned int queue_size) {
	if (!(queue_size && data_size)) {
		errno = EINVAL;
		return -1;
	}

	q->queue_size = queue_size;
	q->end = 0;
	q->start = 0;
	q->data_size = data_size;
	q->data = malloc(data_size * queue_size);
	q->is_empty = true;

	return 0;
}

/**
 * Enqueue element into a dynamic queue
 *
 * @param q - queue to be operated on
 * @param data - data to be added to queue
 *
 * @return error status
 **/
int queue_d_enqueue(queue *q, const void *data) {
	if (data == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (!(q->is_empty || q->start != q->end)) {
		queue_d_resize(q, q->queue_size * _QUEUE_GROWTH_FACTOR);
	}

	memcpy(((char *)q->data) + (q->end * q->data_size), data, q->data_size); //Flawfinder: ignore
	q->end++;
	q->end %= q->queue_size;
	q->is_empty = false;
	return 0;
}

/**
 * Dequeue element from a dynamic queue
 *
 * @param q - queue to be operated on
 * @param data - pointer to memory location for dequeued element to be copied into
 *
 * @return - error status
 **/
int queue_d_dequeue(queue *q, void *data) {
	if (q->is_empty) {
		errno = ENODATA;
		return -1;
	}

	if (data != NULL) {
		memcpy(data, ((char *)q->data) + (q->start * q->data_size), q->data_size); // Flawfinder: ignore
	}

	q->start++;
	q->start %= q->queue_size;
	if (q->start ==q->end) {
		q->is_empty = true;
	}
	// TODO: Decide what to do for case queue becomes empty.  Do I bother resizing?
	if ((q->end - q->start + ((q->end < q->start) * q->queue_size) <=
				q->queue_size / _QUEUE_SHRINK_THRESHOLD_FACTOR) && !q->is_empty) {
		queue_d_resize(q, q->queue_size / _QUEUE_SHRINK_FACTOR);
	}
	return 0;
}

/**
 * Resize dynamic queue
 *
 * @param q - queue to be operated on
 * @param size - new size of th equeue
 *
 * @return - error status
 **/
int queue_d_resize(queue *q, unsigned int size) {
	if (size > q->queue_size) {
		return queue_d_grow(q, size);
	}
	else if (size < q->queue_size) {
		return queue_d_shrink(q, size);
	}

// already correct size.  probably shouldn't happen, but also shouldn't hurt
	return 0;
}

/**
 * Grow dynamic array to new size
 *
 * @param q - queue to be operated on
 * @param size - new size of the queue
 *
 * @return - error status
 **/
inline int queue_d_grow(queue *q, unsigned int size) {
	if (!(q->data = realloc(q->data, size * q->data_size))) {
		return -1;
	}
	// if data wraps around
	if (q->end <= q->start && !q->is_empty) {
		// if the new size is big enought o store the data that wrapped around to the beginning
		if (q->queue_size + q->end < size) {
			memcpy(q->data + (q->data_size * q->queue_size), q->data, q->data_size * q->end);
			q->end = q->queue_size + q->end;
		}
		else {
			unsigned int diff = size - q->queue_size;
			unsigned int new_end = q->end - diff;
			memcpy(q->data + (q->data_size * (q->queue_size)), q->data, q->data_size * diff);
			memmove(q->data, q->data + (q->data_size * diff), q->data_size * new_end);
			q->end = new_end;
		}
	}
	q->queue_size = size;
	return 0;
}

/**
 * Shrink dynamic array to new size
 *
 * @param q - queue to be operated on
 * @param size - new size of the queue
 *
 * @return - error status
 **/
inline int queue_d_shrink(queue *q, unsigned int size) {
	// should fail if trying to shrink queue to a size that does not hold current data
	// TODO: make sure this iff statement is correct for when queue is full and empty
	if (q->end - q->start + ((!q->is_empty && (q->end <= q->start)) * q->queue_size) > size) {
		errno = -1; // no suitable errno, so this will have to do
		return -1;
	}
	// TODO: more efficient copying can probably be done if you're not lazy

	// if the data wraps around
	if (q->end <= q->start && !q->is_empty) {
		unsigned int prewrap_size = q->queue_size - q->start; //check the math on this
		memmove(q->data + q->data_size * (q->queue_size - prewrap_size),
				q->data + q->data_size * (size - prewrap_size), q->data_size * prewrap_size);
		q->start = size - prewrap_size;
	}
	else if (q->end > size) {
		memmove(q->data, q->data + q->data_size * q->start, q->data_size * (q->end - q->start));
		q->end = q->end - q->start;
		q->start = 0;
	}
	if (!(q->data = realloc(q->data, q->data_size * size))) {
		return -1;
	}
	q->queue_size = size;
	return 0;
}

