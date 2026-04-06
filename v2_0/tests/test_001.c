#include "malloc.h"

int main() {
    void *p = malloc(100);
    show_alloc_mem();
    free(p);
    show_alloc_mem();
    p = malloc(100);
    show_alloc_mem();
    free(p);
}