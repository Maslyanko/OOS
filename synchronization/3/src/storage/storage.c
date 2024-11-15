#include <pthread.h>
#include <malloc.h>
#include <string.h>

#include "storage.h"

Node* createNode(const char* value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    
    newNode->value = malloc(strlen(value) + 1);
    strcpy(newNode->value, value);
    newNode->next = NULL;

    initLock(&newNode->lock);

    return newNode;
}

void destroyNode(Node* node) {
    destroyLock(&node->lock);
    free(node);
}

void rlockNode(Node* node) {
    readLock(&node->lock);
}

void wlockNode(Node* node) {
    writeLock(&node->lock);
}

void unlockNode(Node* node) {
    unlock(&node->lock);
}


void initStorage(Storage* storage) {
    storage->head = NULL;
    initLock(&storage->lock);
}

void destroyStorage(Storage* storage) {
    Node* current = storage->head;
    while (current!= NULL) {
        Node* temp = current;
        current = current->next;
        destroyNode(temp);
    }
    storage->head = NULL;
}


void push(Storage* storage, const char* value) {
    Node* newNode = createNode(value);

    if (storage->head) {
        newNode->next = storage->head;
        storage->head = newNode;
    } else {
        storage->head = newNode;
    }
}

char* pop(Storage* storage) {
    if (storage->head) {
        char* value = storage->head->value;
        Node* tmp = storage->head;
        storage->head = storage->head->next;
        destroyNode(tmp);
        return value;
    }

    return NULL;
}

void rlockStorage(Storage* storage) {
    readLock(&storage->lock);
}

void wlockStorage(Storage* storage) {
    writeLock(&storage->lock);
}

void unlockStorage(Storage* storage) {
    unlock(&storage->lock);
}