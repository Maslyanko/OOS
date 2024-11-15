#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "util.h"

static char RANDOM_ININTED = 0;

int generateRandomInt(int min, int max) {
    if (!RANDOM_ININTED) {
        srand(time(0));
        RANDOM_ININTED = 1;
    }
    return rand() % (max - min + 1) + min;
}

const char* generateRandomString(int length) {
    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    char* randomString = (char*) malloc(length + 1);

    for (int i = 0; i < length; i++) {
        randomString[i] = charset[generateRandomInt(0, 62)];
    }

    randomString[length] = '\0';
    return randomString;
}