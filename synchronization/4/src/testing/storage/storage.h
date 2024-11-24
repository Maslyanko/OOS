#pragma once
#include "../lock/lock.h"

typedef struct _Node {
    char* value;
    struct _Node* next;
    Lock lock;
} Node;

Node* createNode(const char* value);
void destroyNode(Node* node);


void rlockNode(Node* node);
void wlockNode(Node* node);
void unlockNode(Node* node);


typedef struct _Storage {
    Node* head;
    Lock lock;
} Storage;

void initStorage(Storage* storage);

void destroyStorage(Storage* storage);


//thread-unsafe function. Copy value and insert it in storage head.
void push(Storage* storage, const char* value);

//thread-unsafe function. Pop storage head. Return NULL if empty. 
char* pop(Storage* storage);


void rlockStorage(Storage* storage);
void wlockStorage(Storage* storage);
void unlockStorage(Storage* storage);