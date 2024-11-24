#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../../spinlock/spinlock.h"
#include "../lock.h"

void initLock(Lock* lock) {
    lock->lock = (Spinlock*) malloc(sizeof(Spinlock));
    initSpinlock(lock->lock);
}

void destroyLock(Lock* lock) {
    destroySpinlock(lock->lock);
    free(lock->lock);
}

void readLock(Lock* lock) {
    lockSpinlock(lock->lock);
}

void writeLock(Lock* lock) {
    lockSpinlock(lock->lock);
}

void unlock(Lock* lock) {
    unlockSpinlock(lock->lock);
}