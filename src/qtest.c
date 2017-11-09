#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

#define RUN_TEST(test) \
	do { \
		errno = 0; \
		if (test) { \
			ret++; \
			perror(#test); \
		} \
	} while (0)

int test_init(void);
int test_init_null(void);
int test_init_data_size_zero(void);
int test_init_queue_size_zero(void);
int test_enqueue(void);
int test_enqueue_full(void);
int test_dequeue(void);
int test_enqueue_wrap(void);
int test_enqueue_null(void);
int test_dequeue_empty(void);
int test_dequeue_null(void);

int test_d_init(void);
int test_init_d_data_size_zero(void);
int test_init_d_queue_size_zero(void);
int test_d_enqueue(void);
int test_d_enqueue_grow(void);
int test_d_dequeue(void);
int test_d_dequeue_null(void);
int test_d_enqueue_null(void);
int test_d_enqueue_wrap(void);
int test_d_dequeue_shrink(void);

int main(int argc, char **argv) {

	int err = 0;
	unsigned int ret = 0;

	RUN_TEST(test_init());
	RUN_TEST(test_init_null());
	RUN_TEST(test_init_data_size_zero());
	RUN_TEST(test_init_queue_size_zero());
	RUN_TEST(test_enqueue());
	RUN_TEST(test_enqueue_full());
	RUN_TEST(test_dequeue());
	RUN_TEST(test_enqueue_wrap());
	RUN_TEST(test_enqueue_null());
	RUN_TEST(test_dequeue_empty());
	RUN_TEST(test_dequeue_null());

	RUN_TEST(test_d_init());
	RUN_TEST(test_init_d_data_size_zero());
	RUN_TEST(test_init_d_queue_size_zero());
	RUN_TEST(test_d_enqueue());
	RUN_TEST(test_d_enqueue_grow());
	RUN_TEST(test_d_dequeue());
	RUN_TEST(test_d_dequeue_null());
	RUN_TEST(test_d_enqueue_null());
	RUN_TEST(test_d_enqueue_wrap());
	RUN_TEST(test_d_dequeue_shrink());

	if (!ret) {
		printf("Passed all tests\n");
	}
	else {
		printf("Failed %u tests\n", ret);
	}
	return ret;
}

int test_init(void) {
	const int QUEUE_SIZE = 1;
	queue q;
	int data[QUEUE_SIZE];
	int err = queue_init(&q, data, sizeof(int), QUEUE_SIZE);
	return err;
}

int test_init_null(void) {
	const int QUEUE_SIZE = 1;
	queue q;
	int err = queue_init(&q, NULL, sizeof(int), QUEUE_SIZE);
	return !(err && errno == EINVAL);
}

int test_init_data_size_zero(void) {
	const int QUEUE_SIZE = 1;
	queue q;
	int data[QUEUE_SIZE];
	int err = queue_init(&q, data, 0, QUEUE_SIZE);
	return !(err && errno == EINVAL);
}

int test_init_queue_size_zero(void) {
	const int QUEUE_SIZE = 0;
	queue q;
	int data;
	int err = queue_init(&q, &data, sizeof(int), QUEUE_SIZE);
	return !(err && errno == EINVAL);
}

int test_enqueue(void) {
	const int QUEUE_SIZE = 1;
		queue q;
	int data[QUEUE_SIZE];
	int x = 0;
	queue_init(&q, data, sizeof(int), QUEUE_SIZE);
	return queue_enqueue(&q, &x);
}

int test_dequeue(void) {
	const int QUEUE_SIZE = 1;
	queue q;
	int data[QUEUE_SIZE];
	queue_init(&q, data, sizeof(int), QUEUE_SIZE);
	int x = 0;
	int y;
	int err = 0;
	queue_enqueue(&q, &x);
	err |= queue_dequeue(&q, &y);
	err |= (y !=x);
	return err;
}

int test_enqueue_full(void) {
	const int QUEUE_SIZE = 1;
	queue q;
	int data[QUEUE_SIZE];
	int x[QUEUE_SIZE];
	queue_init(&q, data, sizeof(int), QUEUE_SIZE);
	queue_enqueue(&q, &x[0]);
	errno = 0;
	queue_enqueue(&q, &x[0]);
	return (errno != ENOBUFS);
}

int test_enqueue_wrap(void) {
	const int QUEUE_SIZE = 2;
	queue q;
	int data[QUEUE_SIZE];
	int x[2] = {0,1};
	queue_init(&q, data, sizeof(int), QUEUE_SIZE);
	int err = 0;
	err |= queue_enqueue(&q, &x[0]);
	err |= queue_enqueue(&q, &x[1]);
	err |= queue_dequeue(&q, NULL);
	err |= queue_enqueue(&q, &x[0]);
	return err;
}

int test_enqueue_null(void) {
	const int QUEUE_SIZE = 1;
	queue q;
	int data[QUEUE_SIZE];
	queue_init(&q, data, sizeof(int ), QUEUE_SIZE);
	int err = queue_enqueue(&q, NULL);
	return !(err && errno == EINVAL);
}

int test_dequeue_null(void) {
	const int QUEUE_SIZE = 1; queue q;
	int data[QUEUE_SIZE];
	queue_init(&q, data, sizeof(int), QUEUE_SIZE);
	int x = 0;queue_enqueue(&q, &x);
	int err = queue_dequeue(&q, NULL);
	return (err | !q.is_empty);
}

int test_dequeue_empty(void) {
	const int QUEUE_SIZE = 1;
	queue q;
	int data[QUEUE_SIZE];
	queue_init(&q, data, sizeof(int), QUEUE_SIZE);
	int err = queue_dequeue(&q, NULL);
	return !(err && errno == ENODATA);
}

int test_d_init(void) {
	queue q;
	int err = queue_d_init(&q, sizeof(int), 5);
	return err;
}

int test_init_d_data_size_zero(void) {
	queue q;
	int err = queue_d_init(&q, sizeof(int), 0);
	return !(err &&errno == EINVAL);
}

int test_init_d_queue_size_zero(void) {
	queue q;
	int err = queue_d_init(&q, sizeof(int), 0);
	return !(err && errno == EINVAL);
}

int test_d_enqueue(void) {
	queue q;
	queue_d_init(&q, sizeof(int), 1);
	int x = 0;
	return queue_d_enqueue(&q, &x);
}

int test_d_enqueue_grow(void) {
	queue q;
	queue_d_init(&q, sizeof(int), 2);
	int x[4] = {0,1,2,3};
	int err;
	err = queue_d_enqueue(&q, &x[0]);
	err |= queue_d_enqueue(&q, &x[1]);
	err |= queue_d_enqueue(&q, &x[2]);
	err |= queue_d_enqueue(&q, &x[3]);
	return err;
}

int test_d_dequeue(void) {
	queue q;
	queue_d_init(&q, sizeof(int), 1);
	int x = 1;
	int y = 0;
	queue_d_enqueue(&q, &x);
	int err = queue_d_dequeue(&q, &y);
	if (err || y!=x) {
		return -1;
	}
	return 0;
}

int test_d_dequeue_null(void) {
	queue q;
	queue_d_init(&q, sizeof(int), 1);
	int x = 0;
	queue_d_enqueue(&q, &x);
	return queue_d_dequeue(&q, NULL);
}

int test_d_enqueue_null(void) {
	queue q;
	queue_d_init(&q, sizeof(int), 1);
	int err = queue_d_enqueue(&q, NULL);
	return !(err && errno == EINVAL);
}

int test_d_enqueue_wrap(void) {
	queue q;
	queue_d_init(&q, sizeof(int), 3);
	int x[4] = {0,1,2,3};
	queue_d_enqueue(&q, &x[0]);
	queue_d_dequeue(&q, NULL);
	queue_d_enqueue(&q, &x[1]);
	queue_d_enqueue(&q, &x[2]);
	queue_d_enqueue(&q, &x[3]);
	return (q.end < q.start);
}

int test_d_dequeue_shrink(void) {
	const int QUEUE_SIZE = 4;
	queue q;
	queue_d_init(&q, sizeof(int), QUEUE_SIZE);
	for (int i = 0; i < QUEUE_SIZE; i++) {
		queue_d_enqueue(&q, &i);
	}
	for (int i = QUEUE_SIZE - 1; i >= QUEUE_SIZE / _QUEUE_SHRINK_THRESHOLD_FACTOR; i--) {
		queue_d_dequeue(&q, NULL);
	}
	int ret = !(q.queue_size == (QUEUE_SIZE / _QUEUE_SHRINK_FACTOR));

	for (int i = 1; i < QUEUE_SIZE; i++) {
		queue_d_enqueue(&q, &i);
	}

	for (int i = QUEUE_SIZE - 1; i >= QUEUE_SIZE / _QUEUE_SHRINK_THRESHOLD_FACTOR; i--) {
		queue_d_dequeue(&q, NULL);
	}

	ret |= !(q.queue_size == (QUEUE_SIZE / _QUEUE_SHRINK_FACTOR));

	return ret;
}
