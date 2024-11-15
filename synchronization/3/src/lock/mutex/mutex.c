#define _GNU_SOURCE
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../lock.h"

void initLock(Lock* lock) {
    lock->lock = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
    int err = pthread_mutex_init(lock->lock, NULL);
    if (err) {
        printf("Mutex init failed: %s\n", strerror(err));
        abort();
    }
}

void destroyLock(Lock* lock) {
    int err = pthread_mutex_destroy(lock->lock);
    if (err) {
        printf("Mutex destroy failed: %s\n", strerror(err));
        abort();
    }
    free(lock->lock);
}

void readLock(Lock* lock) {
    int err = pthread_mutex_lock(lock->lock);
    if (err) {
        printf("Mutex read lock failed: %s\n", strerror(err));
        abort();
    }
}

void writeLock(Lock* lock) {
    int err = pthread_mutex_lock(lock->lock);
    if (err) {
        printf("Mutex write lock failed: %s\n", strerror(err));
        abort();
    }
}

void unlock(Lock* lock) {
    int err = pthread_mutex_unlock(lock->lock);
    if (err) {
        printf("Mutex unlock failed: %s\n", strerror(err));
        abort();
    }
}