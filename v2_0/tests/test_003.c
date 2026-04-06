#include "malloc.h"

int main() {
    // Граничные значения TINY
    void *t1 = malloc(1);
    void *t2 = malloc(128); // Максимум для TINY (зависит от твоей реализации)
    
    // Граничные значения SMALL
    void *s1 = malloc(129);
    void *s2 = malloc(1024); // Типичный SMALL
    
    // LARGE (напрямую mmap)
    void *l1 = malloc(1024 * 100); // 100 KB
    void *l2 = malloc(1024 * 1024 * 10); // 10 MB
    
    show_alloc_mem();
    
    free(t1); free(t2);
    free(s1); free(s2);
    free(l1); free(l2);
    return (0);
}