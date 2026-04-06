#include "malloc.h"

int main() {
    void *p[10];
    
    // Выделяем 10 блоков по 64 байта
    for (int i = 0; i < 10; i++) p[i] = malloc(64);
    
    // Освобождаем все блоки
    for (int i = 0; i < 10; i++) free(p[i]);
    
    // Если слияние (coalescing) работает, 
    // этот malloc(500) должен занять место тех самых 10 блоков в той же зоне,
    // а не создавать новую зону или толкать указатель дальше.
    void *big = malloc(500);
    
    show_alloc_mem();
    free(big);
    return (0);
}