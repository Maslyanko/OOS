#pragma once
#include <stdatomic.h>


typedef atomic_int Spinlock;

void initSpinlock(Spinlock* spinlock);

void destroySpinlock(Spinlock* spinlock);

void lockSpinlock(Spinlock* spinlock);

void unlockSpinlock(Spinlock* spinlock);