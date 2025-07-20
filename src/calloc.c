#include "malloc.h"

void    ft_bzero(void *ptr, size_t size) {
    unsigned char *p = ptr;
    while (size--)
        *p++ = 0;
}

void    *calloc(size_t nmemb, size_t size) {
    size_t total = nmemb * size;
    void *ptr = malloc(total);
    if (ptr)
        ft_bzero(ptr, total);
    return ptr;

    // if (nmemb == 0 || size == 0)
    //     return NULL;

    // size_t total_size = nmemb * size;

    // if (nmemb != 0 && total_size / nmemb != size)
    //     return NULL;

    // void    *ptr = malloc(total_size);
    // if (!ptr)
    //     return NULL;

    // ft_bzero(ptr, total_size);
    // return ptr;
}