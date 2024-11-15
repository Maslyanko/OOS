#define _GNU_SOURCE
#include <pthread.h>
#include <assert.h>

#include "queue.h"

static void err_handler(const char* message, int err) {
	if (err != 0) {
		printf("%s%s\n", message, strerror(errno));
        abort();
	}
}

static void init_lock(q_lock* lock, int size) {
	err_handler("Cannot init semaphore: ", sem_init(&lock->access, 0, 1));
	err_handler("Cannot init semaphore: ", sem_init(&lock->writeable, 0, size));
	err_handler("Cannot init semaphore: ", sem_init(&lock->readable, 0, 0));
}

static void destroy_lock(q_lock* lock) {
	err_handler("Cannot destroy semaphore: ", sem_destroy(&lock->access));
	err_handler("Cannot destroy semaphore: ", sem_destroy(&lock->readable));
	err_handler("Cannot destroy semaphore: ", sem_destroy(&lock->writeable));
}

static void lock(q_lock* lock) {
	err_handler("Cannot acquire semaphore: ", sem_wait(&lock->access));
}

static void unlock(q_lock* lock) {
	err_handler("Cannot release semphore: ", sem_post(&lock->access));
}

void* qmonitor(void *arg) {
	queue_t *q = (queue_t *)arg;

	printf("qmonitor: [%d %d %d]\n", getpid(), getppid(), gettid());

	while (1) {
		queue_print_stats(q);
		sleep(1);
	}

	return NULL;
}

queue_t* queue_init(int max_count) {
	int err;

	queue_t *q = malloc(sizeof(queue_t));
	if (!q) {
		printf("Cannot allocate memory for a queue\n");
		abort();
	}

	q->first = NULL;
	q->last = NULL;
	q->max_count = max_count;
	q->count = 0;

	q->add_attempts = q->get_attempts = 0;
	q->add_count = q->get_count = 0;

	init_lock(&q->lock, max_count);

	err = pthread_create(&q->qmonitor_tid, NULL, qmonitor, q);
	if (err) {
		printf("queue_init: pthread_create() failed: %s\n", strerror(err));
		abort();
	}

	return q;
}

void queue_destroy(queue_t *q) {
	int err;
    
	err = pthread_cancel(q->qmonitor_tid);
	if (err) {
		printf("queue_destroy: pthread_cancel() failed: %s\n", strerror(err));
        abort();
	}

	err = pthread_join(q->qmonitor_tid, NULL);
	if (err) {
        printf("queue_destroy: pthread_join() failed: %s\n", strerror(err));
        abort();
    }

	qnode_t *node = q->first;
	while (node) {
        qnode_t *next = node->next;
        free(node);
        node = next;
    }

	destroy_lock(&q->lock);
}

int queue_add(queue_t *q, int val) {
	q->add_attempts++;

    sem_wait(&q->lock.writeable);

    lock(&q->lock);
	{
	    assert(q->count <= q->max_count);
    
	    qnode_t *new = malloc(sizeof(qnode_t));
	    if (!new) {
	    	printf("Cannot allocate memory for new node\n");
	    	abort();
	    }
    
	    new->val = val;
	    new->next = NULL;
    
	    if (!q->first)
	    	q->first = q->last = new;
	    else {
	    	q->last->next = new;
	    	q->last = q->last->next;
	    }
    
	    q->count++;
	    q->add_count++;
	}
	unlock(&q->lock);

	sem_post(&q->lock.readable);

	return 1;
}

int queue_get(queue_t *q, int *val) {
	q->get_attempts++;

    sem_wait(&q->lock.readable);

    lock(&q->lock);
	{
	    assert(q->count >= 0);

	    qnode_t *tmp = q->first;

	    *val = tmp->val;
	    q->first = q->first->next;

	    free(tmp);
	    q->count--;
	    q->get_count++;
	}
	unlock(&q->lock);

    sem_post(&q->lock.writeable);

	return 1;
}

void queue_print_stats(queue_t *q) {
	printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld %ld %ld)\n",
		q->count,
		q->add_attempts, q->get_attempts, q->add_attempts - q->get_attempts,
		q->add_count, q->get_count, q->add_count -q->get_count);
}