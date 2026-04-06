#include "malloc.h"
#include <stdio.h>

int main() {
    for (int i = 1; i <= 100; i++) {
        void *p = malloc(i);
        if (((unsigned long)p % 16) != 0) {
            printf("Fail at size %d: address %p is not 16-byte aligned!\n", i, p);
        }
        free(p);
    }
    printf("Alignment test finished.\n");
    return (0);
}