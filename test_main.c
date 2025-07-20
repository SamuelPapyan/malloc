#include <stdio.h>
#include <stdlib.h>

int main() {
    char *a = malloc(100);
    char *b = malloc(200);
    free(a);
    show_alloc_mem();
    free(b);
    show_alloc_mem();
    return 0;
}
