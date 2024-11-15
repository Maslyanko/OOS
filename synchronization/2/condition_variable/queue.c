#define _GNU_SOURCE
#include <pthread.h>
#include <assert.h>

#include "queue.h"

static void err_handler(const char* message, int err) {
	if (err != 0) {
		printf("%s%s\n", message, strerror(err));
        abort();
	}
}

static void init_lock(q_lock* lock) {
	err_handler("Cannot init mutex: ", pthread_mutex_init(&lock->mutex, NULL));
	err_handler("Cannot init condition variable: ", pthread_cond_init(&lock->not_empty, NULL));
	err_handler("Cannot init condition variable: ", pthread_cond_init(&lock->not_full, NULL));
}

static void destroy_lock(q_lock* lock) {
	err_handler("Cannot destroy mutex: ", pthread_mutex_destroy(&lock->mutex));
	err_handler("Cannot destroy condition variable: ", pthread_cond_destroy(&lock->not_empty));
	err_handler("Cannot destroy condition variable: ", pthread_cond_destroy(&lock->not_full));
}

static void lock(pthread_mutex_t* lock) {
	err_handler("Cannot acquire mutex: ", pthread_mutex_lock(lock));
}

static void unlock(pthread_mutex_t* lock) {
	err_handler("Cannot release mutex: ", pthread_mutex_unlock(lock));
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

	init_lock(&q->lock);

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

    lock(&q->lock.mutex); 
	{
	    assert(q->count <= q->max_count);
    
	    if (q->count == q->max_count) {
			err_handler("Wait failed: ", pthread_cond_wait(&q->lock.not_full, &q->lock.mutex));
		}
    
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


        err_handler("Send signal failed: ", pthread_cond_signal(&q->lock.not_empty));
	}
	unlock(&q->lock.mutex);

	return 1;
}

int queue_get(queue_t *q, int *val) {
	q->get_attempts++;

    lock(&q->lock.mutex); 
	{
	    assert(q->count >= 0);

	    if (q->count == 0) {
			err_handler("Wait failed: ", pthread_cond_wait(&q->lock.not_empty, &q->lock.mutex));
		}

	    qnode_t *tmp = q->first;

	    *val = tmp->val;
	    q->first = q->first->next;

	    free(tmp);
	    q->count--;
	    q->get_count++;

		err_handler("Send signal failed: ", pthread_cond_signal(&q->lock.not_full));
	}
	unlock(&q->lock.mutex);

	return 1;
}

void queue_print_stats(queue_t *q) {
	printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld %ld %ld)\n",
		q->count,
		q->add_attempts, q->get_attempts, q->add_attempts - q->get_attempts,
		q->add_count, q->get_count, q->add_count -q->get_count);
}