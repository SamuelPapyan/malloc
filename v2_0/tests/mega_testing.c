#include "malloc.h"
#include <pthread.h>

void print_test_header(char *str) {
    ft_putstr("\n--- ");
    ft_putstr(str);
    ft_putendl(" ---");
}

void test_fragmentation() {
    print_test_header("Fragmentation & Coalescing");
    void *p1 = malloc(16);
    void *p2 = malloc(16);
    void *p3 = malloc(16);
    
    free(p2); // Создаем дырку посередине
    show_alloc_mem(); 

    free(p1); // Должно произойти слияние p1 и p2
    free(p3); // Должно произойти слияние с p3. Итог: одна пустая зона.
    
    void *p4 = malloc(48); // Если слияние работает, этот блок займет место p1+p2+p3
    show_alloc_mem();
    free(p4);
}

void test_alignment() {
    print_test_header("Alignment Test");
    size_t sizes[] = {1, 5, 17, 33, 65, 120};
    for (int i = 0; i < 6; i++) {
        void *p = malloc(sizes[i]);
        unsigned long addr = (unsigned long)p;
        ft_putstr("Size ");
        ft_putnbr(sizes[i]);
        ft_putstr(": Address ");
        ft_putptr(p);
        if (addr % 16 == 0) ft_putendl(" - OK (16-byte aligned)");
        else ft_putendl(" - ERROR (Not aligned!)");
        free(p);
    }
}

void test_many_allocs() {
    print_test_header("Stress Test: 200 TINY blocks");
    void *ptrs[200];
    for (int i = 0; i < 200; i++) {
        ptrs[i] = malloc(32);
    }
    // Должно быть как минимум 2 зоны TINY (так как в одной обычно умещается ~100-128 блоков)
    show_alloc_mem();
    
    for (int i = 0; i < 200; i++) {
        free(ptrs[i]);
    }
    ft_putendl("All 200 blocks freed.");
    show_alloc_mem(); // Тут должна сработать твоя функция should_unmap_zone
}

void test_edge_cases() {
    print_test_header("Edge Cases");
    
    void *p1 = malloc(0);
    ft_putstr("malloc(0): ");
    ft_putptr(p1); // Может быть NULL или уникальный указатель
    ft_putendl("");

    free(NULL); // Не должно вызывать сегфолт
    ft_putendl("free(NULL) - Passed");

    void *p2 = realloc(NULL, 128);
    if (p2) ft_putendl("realloc(NULL, 128) - Passed");
    free(p2);
}

void *thread_func(void *arg) {
    (void)arg;
    for (int i = 0; i < 50; i++) {
        void *p = malloc(64);
        free(p);
    }
    return NULL;
}

void test_threads() {
    print_test_header("Multi-threading Test");
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_func, NULL);
    pthread_create(&t2, NULL, thread_func, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    ft_putendl("Threads finished without deadlock.");
}

int main() {
    test_fragmentation();
    test_alignment();
    test_many_allocs();
    test_edge_cases();
    test_threads();
    return 0;
}