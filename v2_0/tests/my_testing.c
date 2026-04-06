#include "malloc.h"

int main() {
    // char *str1 = malloc(11);
    // show_alloc_mem();
    // char *str2 = malloc(12);
    // show_alloc_mem();
    // free(str1);
    // show_alloc_mem();
    // str1 = malloc(5000);
    // show_alloc_mem();
    void *p1 = malloc(7);
    void *p2 = malloc(7);
    void *p3 = malloc(7);
    void *p5 = malloc(129);
    void *p6 = malloc(20480);
    void *p7 = malloc(1025);
    show_alloc_mem();
    free(p1);
    free(p2);
    free(p3);
    free(p5);
    free(p6);
    void *p4 = malloc(48);
    show_alloc_mem();
}