#include "malloc.h"

void print_test_header(char *str) {
    ft_putstr("\n--- ");
    ft_putstr(str);
    ft_putendl(" ---");
}

int main() {
    print_test_header("Test 1: Malloc 16 bytes and Fill");
    char *ptr = (char *)malloc(16);
    strcpy(ptr, "Hello, World!");
    ft_putstr("Data: ");
    ft_putendl(ptr);
    show_alloc_mem();

    print_test_header("Test 2: Realloc to LARGE (Upsize to 2048)");
    // Это заставит библиотеку выделить новую зону, скопировать данные и сделать free старой
    ptr = (char *)realloc(ptr, 2048);
    ft_putstr("Data after realloc: ");
    ft_putendl(ptr); // Проверяем, сохранились ли данные
    show_alloc_mem();

    print_test_header("Test 3: Realloc to SMALL (Downsize to 512)");
    // В твоей реализации это просто вернет тот же указатель, так как 2048 > 512
    ptr = (char *)realloc(ptr, 512);
    ft_putstr("Data after downsize: ");
    ft_putendl(ptr);
    show_alloc_mem();

    print_test_header("Test 4: Realloc NULL (Acts like Malloc)");
    void *ptr2 = realloc(NULL, 64);
    if (ptr2) ft_putendl("Success: realloc(NULL, 64) worked.");
    show_alloc_mem();

    print_test_header("Test 5: Realloc to 0 (Acts like Free)");
    realloc(ptr2, 0);
    ft_putendl("ptr2 freed via realloc(ptr, 0)");
    show_alloc_mem();

    print_test_header("Final Clean up");
    free(ptr);
    show_alloc_mem();

    return 0;
}