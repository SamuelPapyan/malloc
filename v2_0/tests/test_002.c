#include <stdio.h>
#include <string.h>
#include "malloc.h"

int main() {
    printf("--- ТЕСТ 1: Проверка TINY (128 блоков) ---\n");
    void *ptrs[128];
    for (int i = 0; i < 128; i++) {
        ptrs[i] = malloc(16); // Заполняем одну зону TINY целиком
    }
    show_alloc_mem();

    printf("\n--- ТЕСТ 2: Coalescing (Освобождение через один) ---\n");
    for (int i = 0; i < 128; i += 2) {
        free(ptrs[i]);
    }
    show_alloc_mem();

    printf("\n--- ТЕСТ 3: Переход TINY -> SMALL -> LARGE (Realloc) ---\n");
    void *re-p = malloc(32);
    strcpy(re-p, "Hello, Malloc!");
    printf("До: %p [%s]\n", re-p, (char*)re-p);
    
    re-p = realloc(re-p, 512); // Должен уйти в SMALL
    printf("После SMALL: %p [%s]\n", re-p, (char*)re-p);
    
    re-p = realloc(re-p, 4096); // Должен уйти в LARGE
    printf("После LARGE: %p [%s]\n", re-p, (char*)re-p);
    show_alloc_mem();

    printf("\n--- ТЕСТ 4: Очистка зон (Unmapping) ---\n");
    // Выделяем две зоны SMALL
    void *s1 = malloc(1024);
    void *s2 = malloc(1024);
    show_alloc_mem();
    
    printf("Освобождаем всё...\n");
    free(s1);
    free(s2);
    free(re-p);
    for (int i = 1; i < 128; i += 2) free(ptrs[i]);

    show_alloc_mem(); 
    // В итоге должна остаться максимум 1 пустая зона каждого типа (или 0)
    
    return 0;
}