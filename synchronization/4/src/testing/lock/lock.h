#pragma once

typedef struct Lock {
    void* lock;
} Lock;

void initLock(Lock* lock);

void destroyLock(Lock* lock);

void readLock(Lock* lock);
void writeLock(Lock* lock);

void unlock(Lock* lock);