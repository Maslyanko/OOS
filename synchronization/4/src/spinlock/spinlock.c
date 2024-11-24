#include "spinlock.h"
#include <stdatomic.h>

void initSpinlock(Spinlock* spinlock) {
    atomic_store(spinlock, 0);
}

void destroySpinlock(Spinlock* spinlock) {}

void lockSpinlock(Spinlock* spinlock) {
    while (1) {
        int expected = 0;

        if (atomic_compare_exchange_strong(spinlock, &expected, 1)) {
            return;
        }
    }
}

void unlockSpinlock(Spinlock* spinlock) {
    int expected = 1;
    atomic_compare_exchange_strong(spinlock, &expected, 0);
}
