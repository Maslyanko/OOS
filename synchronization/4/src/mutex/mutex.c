#include "mutex.h"
#include <linux/futex.h>
#include <unistd.h>
#include <sys/syscall.h>

void initMutex(Mutex* mutex) {
    atomic_store(mutex, 0);
}

void destroyMutex(Mutex* mutex) {}

void lockMutex(Mutex* mutex) {
    int compare = 0;
    atomic_compare_exchange_strong(mutex, &compare, 1);
    
    if (compare != 0) {
        do {
            int tmp = 1;
            if (compare == 2 || atomic_compare_exchange_strong(mutex, &tmp, 2)) {
                syscall(SYS_futex, (int*)&mutex, FUTEX_WAIT, 2, 0, 0, 0);
            }
            compare = 0;
            atomic_compare_exchange_strong(mutex, &compare, 2);
        } while (compare != 0);
    }
}

void unlockMutex(Mutex* mutex) {
    if (atomic_fetch_sub(mutex, 1) != 1) {
        atomic_store(mutex, 0);
        syscall(SYS_futex, (int*)&mutex, FUTEX_WAKE, 1, 0, 0, 0);
    }
}
