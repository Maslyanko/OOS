#pragma once
#include <stdatomic.h>

typedef atomic_int Mutex;

void initMutex(Mutex* mutex);

void destroyMutex(Mutex* mutex);

void lockMutex(Mutex* mutex);

void unlockMutex(Mutex* mutex);