#include "malloc.h" // Подключите заголовок с объявлениями ваших функций

int main(void) {
    ft_putstr_fd("=== malloc(100) ===\n", 1);
    void *ptr1 = malloc(10000);
    if (ptr1)
        ft_memset(ptr1, 'A', 10);
    // ft_putstr_fd(ptr1, 1);

    // ft_putstr_fd("=== malloc(500) ===\n", 1);
    // void *ptr2 = malloc(500);
    // if (ptr2)
    //     ft_memset(ptr2, 'B', 500);

    // ft_putstr_fd("=== free(ptr1) ===\n", 1);
    // free(ptr1);

    // ft_putstr_fd("=== realloc(ptr2, 1000) ===\n", 1);
    // void *ptr3 = realloc(ptr2, 1000);
    // if (ptr3)
    //     ft_memset(ptr3, 'C', 1000);

    // ft_putstr_fd("=== calloc(10, 50) ===\n", 1);
    // void *ptr4 = calloc(10, 50); // 500 байт нулей

    // ft_putstr_fd("=== show_alloc_mem() ===\n", 1);
    // show_alloc_mem();

    // ft_putstr_fd("=== free all ===\n", 1);
    // free(ptr3);
    // free(ptr4);

    show_alloc_mem();
    free(ptr1);
    ft_putstr_fd("=== show_alloc_mem() after free ===\n", 1);
    show_alloc_mem();

    return 0;
}
