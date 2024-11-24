#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../../mutex/mutex.h"
#include "../lock.h"

void initLock(Lock* lock) {
    lock->lock = (Mutex*) malloc(sizeof(Mutex));
    initMutex(lock->lock);
}

void destroyLock(Lock* lock) {
    destroyMutex(lock->lock);
    free(lock->lock);
}

void readLock(Lock* lock) {
    lockMutex(lock->lock);
}

void writeLock(Lock* lock) {
    lockMutex(lock->lock);
}

void unlock(Lock* lock) {
    unlockMutex(lock->lock);
}