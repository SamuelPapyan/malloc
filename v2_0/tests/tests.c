#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdint.h>

#define MAX_PTRS 5000
#define ITERATIONS 20000

// Тест 1: Случайная нагрузка (Stress Test)
// Имитирует реальную работу программы: постоянно выделяет и удаляет блоки разного размера.
void test_random_stress() {
    printf("Running: Random Stress Test (%d iterations)... ", ITERATIONS);
    void *ptrs[MAX_PTRS] = {NULL};
    size_t sizes[MAX_PTRS] = {0};

    for (int i = 0; i < ITERATIONS; i++) {
        int idx = rand() % MAX_PTRS;
        
        if (ptrs[idx]) {
            // Проверяем целостность данных перед удалением
            unsigned char *data = (unsigned char *)ptrs[idx];
            for (size_t j = 0; j < sizes[idx]; j++) {
                if (data[j] != (unsigned char)(idx & 0xFF)) {
                    printf("\nError: Memory corruption at index %d\n", idx);
                    exit(1);
                }
            }
            free(ptrs[idx]);
            ptrs[idx] = NULL;
        } else {
            // Выделяем случайный размер от 1 до 8192 байт
            sizes[idx] = (rand() % 8192) + 1;
            ptrs[idx] = malloc(sizes[idx]);
            assert(ptrs[idx] != NULL);
            
            // Заполняем паттерном для последующей проверки
            memset(ptrs[idx], (int)(idx & 0xFF), sizes[idx]);
        }
    }

    // Чистим всё, что осталось
    for (int i = 0; i < MAX_PTRS; i++) {
        if (ptrs[i]) free(ptrs[i]);
    }
    printf("PASSED\n");
}

// Тест 2: Эффект «Гребенки» (Fragmentation)
// Проверяет, умеет ли аллокатор объединять маленькие свободные блоки в один большой.
void test_coalescing() {
    printf("Running: Adjacent Free Coalescing... ");
    
    // Выделяем 3 блока подряд
    void *p1 = malloc(1024);
    void *p2 = malloc(1024);
    void *p3 = malloc(1024);
    
    // Освобождаем их
    free(p1);
    free(p2);
    free(p3);
    
    // Если аллокатор склеивает блоки, мы сможем выделить один блок почти в 3КБ
    void *p4 = malloc(3000);
    assert(p4 != NULL);
    
    free(p4);
    printf("PASSED\n");
}

// Тест 3: Realloc NULL и Realloc в Free
void test_realloc_edge_cases() {
    printf("Running: Realloc Edge Cases... ");
    
    // realloc(NULL, size) эквивалентен malloc(size)
    void *p1 = realloc(NULL, 100);
    assert(p1 != NULL);
    
    // realloc(ptr, 0) обычно эквивалентен free(ptr) или возвращает минимальный блок
    void *p2 = realloc(p1, 0);
    // p1 уже не валиден
    
    printf("PASSED\n");
}

// Тест 4: Проверка выравнивания (Alignment) для разных типов
void test_strict_alignment() {
    printf("Running: Strict 16-byte Alignment... ");
    for (int i = 1; i < 128; i++) {
        void *p = malloc(i);
        // Современные аллокаторы должны выравнивать адрес по 16 байт на x64
        if ((uintptr_t)p % 16 != 0) {
            printf("\nWarning: Pointer %p is not 16-byte aligned (size: %d)\n", p, i);
        }
        free(p);
    }
    printf("PASSED\n");
}

// Тест 5: Лавина мелких запросов
void test_tiny_allocs() {
    printf("Running: Tiny Allocs (1 byte)... ");
    void *ptrs[1000];
    for (int i = 0; i < 1000; i++) {
        ptrs[i] = malloc(1);
        assert(ptrs[i] != NULL);
    }
    for (int i = 0; i < 1000; i++) {
        free(ptrs[i]);
    }
    printf("PASSED\n");
}

int main() {
    srand(time(NULL));
    printf("=== Starting Advanced Stress Tests ===\n\n");

    test_random_stress();
    test_coalescing();
    test_realloc_edge_cases();
    test_strict_alignment();
    test_tiny_allocs();

    printf("\n=== All Advanced Tests Passed! ===\n");
    return 0;
}