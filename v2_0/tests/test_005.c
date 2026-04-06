#include "malloc.h"
#include <string.h>

int main() {
    char *ptr = malloc(10);
    strcpy(ptr, "42");
    show_alloc_mem();
    
    // Расширяем внутри TINY
    ptr = realloc(ptr, 64);
    if (strcmp(ptr, "42") != 0) write(1, "Error TINY\n", 11);
    show_alloc_mem();
    
    // Прыжок из TINY в SMALL
    ptr = realloc(ptr, 512);
    if (strcmp(ptr, "42") != 0) write(1, "Error SMALL\n", 12);
    show_alloc_mem();
    
    // Прыжок из SMALL в LARGE
    ptr = realloc(ptr, 8000);
    if (strcmp(ptr, "42") != 0) write(1, "Error LARGE\n", 12);

    show_alloc_mem();
    free(ptr);
    return (0);
}