#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "storage/storage.h"
#include "util/util.h"

unsigned int STORAGE_SIZE = 10;

unsigned int INCR_ITERATIONS = 0;
unsigned int INCR_NUMBER = 0;

unsigned int DECR_ITERATIONS = 0;
unsigned int DECR_NUMBER = 0;

unsigned int EQUAL_ITERATIONS = 0;
unsigned int EQUAL_NUMBER = 0;

pthread_spinlock_t swapSpin;
unsigned int SWAP_NUMBER = 0;

Storage storage;

typedef struct ComparatorThreadArg {
    void (*comparator)(int, int);
    unsigned int* iterations;
} ComparatorThreadArg;

void monitoring() {
    while (1) {
        printf("Incr: %u, Decr: %u, Equal: %u, Swap: %u\n", INCR_NUMBER, DECR_NUMBER, EQUAL_NUMBER, SWAP_NUMBER);
        sleep(1);
    }
}


void incrementSwapNumber() {
    pthread_spin_lock(&swapSpin);
    SWAP_NUMBER++;
    pthread_spin_unlock(&swapSpin);
}

void incrComparator(int len1, int len2) {
    if (len1 < len2) {
        INCR_NUMBER++;
    }
}

void decrComparator(int len1, int len2) {
    if (len1 > len2) {
        DECR_NUMBER++;
    }
}

void equalComparator(int len1, int len2) {
    if (len1 == len2) {
        EQUAL_NUMBER++;
    }
}

void* comparatorThreadFoo(void* arg) {
    ComparatorThreadArg* cmpArg = (ComparatorThreadArg*) arg;
    void (*comparator)(int, int) = cmpArg->comparator;
    unsigned int* iterations = cmpArg->iterations;

    while(1) {
        rlockStorage(&storage);
        Node* first = storage.head;
        unlockStorage(&storage);
        rlockNode(first);

        while (1) {
            Node* second = first->next;

            if (!second) {
                unlockNode(first);
                break;
            }

            comparator(strlen(first->value), strlen(second->value));

            rlockNode(second);
            unlockNode(first);
            first = second;
        }

        ++*iterations;
    }
}

void* swapperThreadFoo(void* arg) {
    Storage* storage = (Storage*) arg;
    Node* current = NULL;

    while(1) {
        Node* next;
        Node* nextNext;

        if (current) {
            wlockNode(current);
            if (current->next) {
                wlockNode(current->next);
                next = current->next;
            } else {
                unlockNode(current);
                current = NULL;
                continue;
            }

            if (next->next) {
                wlockNode(next->next);
                nextNext = next->next;
            } else {
                unlockNode(next);
                unlockNode(current);
                current = NULL;
                continue;
            }

            if (generateRandomInt(0, 100) % 25 == 0) {
                next->next = nextNext->next;
                nextNext->next = next;
                current->next = nextNext;
                incrementSwapNumber();
            }

            unlockNode(nextNext);
            unlockNode(next);
            unlockNode(current);
        } else {
            rlockStorage(storage);
            next = storage->head;
            unlockStorage(storage);

            wlockNode(next);

            if (next->next) {
                wlockNode(next->next);
                nextNext = next->next;
            } else {
                unlockNode(next);
                continue;
            }

            if (generateRandomInt(0, 100) % 25 == 0) {
                next->next = nextNext->next;
                nextNext->next = next;

                wlockStorage(storage);
                storage->head = nextNext;
                unlockStorage(storage);

                incrementSwapNumber();
            }
            
            unlockNode(nextNext);
            unlockNode(next);
        }
        current = next;
    }
}


int main(int argc, char **argv) {
    int err;

    initStorage(&storage);
    
    err = pthread_spin_init(&swapSpin, PTHREAD_PROCESS_PRIVATE);
    if (err) {
        printf("Spinlock init failed: %s\n", strerror(err));
        abort();
    }

    for (int i = 0; i < STORAGE_SIZE; ++i) {
        const char* value = generateRandomString(generateRandomInt(0, 100));
        push(&storage, value);
        free((void*) value);
    }

    pthread_t threads[6];

    ComparatorThreadArg arg1 = {incrComparator, &INCR_ITERATIONS};
    err = pthread_create(&threads[0], NULL, comparatorThreadFoo, &arg1);
    if (err) {
        printf("pthread_create() failed: %s\n", strerror(err));
        abort();
    }

    ComparatorThreadArg arg2 = {decrComparator, &DECR_ITERATIONS};
    err = pthread_create(&threads[1], NULL, comparatorThreadFoo, &arg2);
    if (err) {
        printf("pthread_create() failed: %s\n", strerror(err));
        abort();
    }

    ComparatorThreadArg arg3 = {equalComparator, &EQUAL_ITERATIONS};
    err = pthread_create(&threads[2], NULL, comparatorThreadFoo, &arg3);
    if (err) {
        printf("pthread_create() failed: %s\n", strerror(err));
        abort();
    }

    for (int i = 3; i < 6; i++) {
        err = pthread_create(&threads[i], NULL, swapperThreadFoo, &storage);
        if (err) {
            printf("pthread_create() failed: %s\n", strerror(err));
            abort();
        }
    }

    monitoring();
}