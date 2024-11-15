#define _GNU_SOURCE
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../lock.h"

void initLock(Lock* lock) {
    lock->lock = (pthread_rwlock_t*) malloc(sizeof(pthread_rwlock_t));
    int err = pthread_rwlock_init(lock->lock, NULL);
    if (err) {
        printf("RWLock init failed: %s\n", strerror(err));
        abort();
    }
}

void destroyLock(Lock* lock) {
    int err = pthread_rwlock_destroy(lock->lock);
    if (err) {
        printf("RWLock destroy failed: %s\n", strerror(err));
        abort();
    }
    free(lock->lock);
}

void readLock(Lock* lock) {
    int err = pthread_rwlock_rdlock(lock->lock);
    if (err) {
        printf("RWLock read lock failed: %s\n", strerror(err));
        abort();
    }
}

void writeLock(Lock* lock) {
    int err = pthread_rwlock_wrlock(lock->lock);
    if (err) {
        printf("RWLock write lock failed: %s\n", strerror(err));
        abort();
    }
}

void unlock(Lock* lock) {
    int err = pthread_rwlock_unlock(lock->lock);
    if (err) {
        printf("RWLock unlock failed: %s\n", strerror(err));
        abort();
    }
}