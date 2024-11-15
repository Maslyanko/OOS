#define _GNU_SOURCE
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../lock.h"

void initLock(Lock* lock) {
    lock->lock = malloc(sizeof(pthread_spinlock_t));
    int err = pthread_spin_init(lock->lock, PTHREAD_PROCESS_PRIVATE);
    if (err) {
        printf("Spinlock init failed: %s\n", strerror(err));
        abort();
    }
}

void destroyLock(Lock* lock) {
    int err = pthread_spin_destroy(lock->lock);
    if (err) {
        printf("Spinlock destroy failed: %s\n", strerror(err));
        abort();
    }
    free(lock->lock);
}

void readLock(Lock* lock) {
    int err = pthread_spin_lock(lock->lock);
    if (err) {
        printf("Spinlock read lock failed: %s\n", strerror(err));
        abort();
    }
}
void writeLock(Lock* lock) {
    int err = pthread_spin_lock(lock->lock);
    if (err) {
        printf("Spinlock write lock failed: %s\n", strerror(err));
        abort();
    }
}

void unlock(Lock* lock) {
    int err = pthread_spin_unlock(lock->lock);
    if (err) {
        printf("Spinlock unlock failed: %s\n", strerror(err));
        abort();
    }
}