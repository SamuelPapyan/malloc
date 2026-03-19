#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

// #include "malloc.h"


#define ALIGNMENT 16
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

#define TINY_MAX   64
#define SMALL_MAX  1024

#define MIN_BLOCKS_PER_ZONE 100


typedef struct s_block {
    size_t          size;
    int            is_free;
    struct s_block *next;
} t_block;

typedef struct s_zone {
    size_t          size;
    struct s_zone  *next;
    t_block        *blocks;
} t_zone;

typedef struct s_heap {
    t_zone *tiny;
    t_zone *small;
    t_zone *large;
} t_heap;


static t_heap g_heap = {0};

static size_t pagesize(void) {
    static size_t p = 0;
    if (!p) p = (size_t) sysconf(_SC_PAGESIZE);
    return p;
}


static size_t calc_zone_size_for_block_payload(size_t payload)
{
    size_t ps = pagesize();
    size_t meta_per_block = sizeof(t_block);
    size_t needed = sizeof(t_zone) + (meta_per_block + payload) * MIN_BLOCKS_PER_ZONE;
    size_t zone_size = ((needed + ps - 1) / ps) * ps;
    return zone_size;
}

static int block_belongs_to_zone(t_zone *zone, t_block *block)
{
    if (!zone || !block) return 0;
    char *zstart = (char *)zone;
    char *zend = zstart + zone->size;
    char *baddr = (char *)block;
    return (baddr >= zstart && baddr < zend);
}

static void print_addr(void *addr) {
    printf("0x%lX", (unsigned long) (uintptr_t) addr);
}


static void init_zone_blocks(t_zone *zone, size_t block_payload_size)
{
    if (!zone) return;

    char *cursor = (char *)zone + sizeof(t_zone);
    char *zone_end = (char *)zone + zone->size;

    t_block *prev = NULL;
    while (1) {
        if (cursor + sizeof(t_block) > zone_end) break;
        if (cursor + sizeof(t_block) + block_payload_size > zone_end) break;

        t_block *b = (t_block *)cursor;
        b->size = block_payload_size;
        b->is_free = 1;
        b->next = NULL;

        if (prev)
            prev->next = b;
        else
            zone->blocks = b;

        prev = b;

        cursor = cursor + sizeof(t_block) + block_payload_size;
    }
}

static t_zone *create_zone(size_t block_payload_size)
{
    size_t zone_size = calc_zone_size_for_block_payload(block_payload_size);
    void *mem = mmap(NULL, zone_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (mem == MAP_FAILED)
        return NULL;

    t_zone *zone = (t_zone *) mem;
    zone->size = zone_size;
    zone->next = NULL;
    zone->blocks = NULL;

    init_zone_blocks(zone, block_payload_size);

    return zone;
}

static t_block *find_free_block_in_zone(t_zone *zone, size_t size)
{
    if (!zone) return NULL;
    for (t_block *b = zone->blocks; b != NULL; b = b->next) {
        if (b->is_free && b->size >= size)
            return b;
    }
    return NULL;
}

static t_block *find_free_block_in_list(t_zone *zone_list, size_t size)
{
    for (t_zone *z = zone_list; z; z = z->next) {
        t_block *b = find_free_block_in_zone(z, size);
        if (b) return b;
    }
    return NULL;
}

static void push_zone(t_zone **list, t_zone *zone)
{
    zone->next = *list;
    *list = zone;
}

static void remove_zone_from_list(t_zone **list, t_zone *target)
{
    if (!list || !*list || !target) return;

    t_zone *prev = NULL;
    for (t_zone *z = *list; z; prev = z, z = z->next) {
        if (z == target) {
            if (prev) prev->next = z->next;
            else *list = z->next;
            munmap((void *)z, z->size);
            return;
        }
    }
}

static int zone_all_free(t_zone *zone)
{
    if (!zone) return 1;
    for (t_block *b = zone->blocks; b; b = b->next) {
        if (!b->is_free) return 0;
    }
    return 1;
}


static int merge_with_next_if_possible(t_block *b)
{
    if (!b || !b->next) return 0;
    if (!b->is_free || !b->next->is_free) return 0;

    char *end_of_b = (char *)b + sizeof(t_block) + b->size;
    char *next_meta = (char *)b->next;

    if (end_of_b != next_meta) {
        return 0;
    }

    b->size += sizeof(t_block) + b->next->size;
    b->next = b->next->next;
    return 1;
}

static void defragment_zone(t_zone *zone)
{
    if (!zone) return;

    int changed;
    do {
        changed = 0;
        t_block *cur = zone->blocks;
        while (cur) {
            if (merge_with_next_if_possible(cur)) {
                changed = 1;
                continue;
            }
            cur = cur->next;
        }
    } while (changed);
}

/* Попытка усечь хвост зоны: если есть свободный хвост, его можно munmap'ить.
   Работает только если последний использованный блок находится не в самом конце,
   и границы страниц позволяют.
*/
static void try_shrink_zone(t_zone *zone)
{
    if (!zone) return;

    /* ищем последний блок, который не свободен */
    t_block *last_used = NULL;
    t_block *cur = zone->blocks;
    while (cur) {
        if (!cur->is_free) last_used = cur;
        cur = cur->next;
    }

    /* если все блоки свободны — лучше удалить всю зону (в вызывающей функции) */
    if (!last_used) return;

    char *end_of_last_used = (char *)last_used + sizeof(t_block) + last_used->size;
    char *zone_end = (char *)zone + zone->size;

    if (end_of_last_used >= zone_end) return; // нечего укорачивать

    size_t ps = pagesize();

    /* Начало хвоста — округляем вверх до границы страницы (чтобы munmap принимал) */
    uintptr_t tail_start = (uintptr_t)end_of_last_used;
    uintptr_t tail_page = (tail_start + ps - 1) & ~(ps - 1);
    if ((char *)tail_page >= zone_end) return;

    size_t unmap_size = (size_t)((char *)zone_end - (char *)tail_page);

    /* В целях безопасности: не укорачиваем если это слишком маленькая часть,
       но это можно настроить. */
    if (unmap_size < ps) return;

    /* Выполняем munmap на хвост */
    if (munmap((void *)tail_page, unmap_size) == 0) {
        /* корректируем размер зоны */
        zone->size -= unmap_size;
        /* удостоверимся, что no block meta ссылались за новую границу — init_zone_blocks гарантировал порядок,
           а мы munmap'им целыми страницами, так что след. мета должна быть за новой границей; в реальной системе
           потребуются дополнительные проверки. */
    } else {
        /* munmap failed — ничего не делаем */
    }
}

/* ---------------------------------------- */
/* Основная логика malloc/free/realloc/show */

/* malloc implementation */
void *malloc(size_t size)
{
    if (size == 0) return NULL;
    size = ALIGN(size);

    /* LARGE allocation: отдельная mmap-зона */
    if (size > SMALL_MAX) {
        size_t total = sizeof(t_zone) + sizeof(t_block) + size;
        /* округлить до страницы */
        size_t ps = pagesize();
        size_t zone_size = ((total + ps - 1) / ps) * ps;

        t_zone *zone = mmap(NULL, zone_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
        if (zone == MAP_FAILED) return NULL;
        zone->size = zone_size;
        zone->next = NULL;
        zone->blocks = NULL;

        /* создаём единственный block прямо после t_zone */
        t_block *b = (t_block *)((char *)zone + sizeof(t_zone));
        b->size = size;
        b->is_free = 0;
        b->next = NULL;
        zone->blocks = b;

        push_zone(&g_heap.large, zone);
        return (void *)((char *)b + sizeof(t_block));
    }

    /* Выбираем список зон (tiny или small) */
    t_zone **list;
    size_t block_payload_size;
    if (size <= TINY_MAX) {
        list = &g_heap.tiny;
        block_payload_size = TINY_MAX;
    } else {
        list = &g_heap.small;
        block_payload_size = SMALL_MAX;
    }

    /* Сначала пробуем найти готовый свободный блок в существующих зонах */
    for (t_zone *z = *list; z; z = z->next) {
        t_block *b = find_free_block_in_zone(z, size);
        if (b) {
            b->is_free = 0;
            return (void *)((char *)b + sizeof(t_block));
        }
    }

    /* Нет подходящего блока — создаём новую зону (с payload == block_payload_size) */
    t_zone *new_zone = create_zone(block_payload_size);
    if (!new_zone) return NULL;

    push_zone(list, new_zone);

    /* первый найденный блок — используем его */
    t_block *first = new_zone->blocks;
    if (!first) return NULL; /* на всякий случай */

    /* Если payload блока больше чем нужно — просто используем его полностью.
       (В более сложной реализации можно разбивать block на две части). */
    first->is_free = 0;
    first->size = block_payload_size; /* note: stored size is payload capacity; caller got 'size' bytes */
    return (void *)((char *)first + sizeof(t_block));
}

/* Найти t_zone и t_block по заданному ptr; возвращает зону через параметр zone_out (может быть NULL) */
static t_block *find_block_and_zone(void *ptr, t_zone **zone_out)
{
    if (!ptr) return NULL;
    t_block *block = (t_block *)((char *)ptr - sizeof(t_block));
    /* Check large list first */
    for (t_zone *z = g_heap.large; z; z = z->next) {
        if (block_belongs_to_zone(z, block)) {
            if (zone_out) *zone_out = z;
            return block;
        }
    }
    /* tiny / small */
    for (t_zone *z = g_heap.tiny; z; z = z->next) {
        if (block_belongs_to_zone(z, block)) {
            if (zone_out) *zone_out = z;
            return block;
        }
    }
    for (t_zone *z = g_heap.small; z; z = z->next) {
        if (block_belongs_to_zone(z, block)) {
            if (zone_out) *zone_out = z;
            return block;
        }
    }
    return NULL;
}

/* free implementation (с дефрагментацией и удалением пустых зон) */
void free(void *ptr)
{
    if (!ptr) return;

    t_zone *zone = NULL;
    t_block *block = find_block_and_zone(ptr, &zone);
    if (!block || !zone) {
        /* ptr не найден — игнорируем (как безопасный вариант) */
        return;
    }

    /* Если это large-зона (в ней только один block по design) — полностью удаляем */
    /* Определяем large по тому, что зона есть в списке large */
    int is_large = 0;
    for (t_zone *z = g_heap.large; z; z = z->next) {
        if (z == zone) { is_large = 1; break; }
    }

    if (is_large) {
        remove_zone_from_list(&g_heap.large, zone);
        return;
    }

    /* Иначе — пометим block как свободный, выполним дефрагментацию и при необходимости
       попробуем уменьшить зону или удалить её если полностью пуста. */
    block->is_free = 1;

    /* Коалесценция соседних свободных блоков */
    defragment_zone(zone);

    /* Попробуем усечь хвост зоны и вернуть страницы (если возможно) */
    try_shrink_zone(zone);

    /* Если теперь зона полностью свободна — удалим её */
    if (zone_all_free(zone)) {
        /* определим принадлежность к tiny/small и удалим из соответствующего списка */
        /* tiny? */
        t_zone *z;
        int removed = 0;
        for (z = g_heap.tiny; z; z = z->next) {
            if (z == zone) { remove_zone_from_list(&g_heap.tiny, zone); removed = 1; break; }
        }
        if (!removed) {
            for (z = g_heap.small; z; z = z->next) {
                if (z == zone) { remove_zone_from_list(&g_heap.small, zone); break; }
            }
        }
    }
}

/* realloc implementation */
void *realloc(void *ptr, size_t size)
{
    if (!ptr) return malloc(size);
    if (size == 0) { free(ptr); return NULL; }

    t_zone *zone = NULL;
    t_block *block = find_block_and_zone(ptr, &zone);
    if (!block) {
        /* ptr не наш — безопасный вариант: behave like malloc */
        void *p = malloc(size);
        if (p) {
            /* no copy because we don't know original size; in other impl you could try memmove */
        }
        return p;
    }

    if (block->size >= size) {
        /* старый блок вмещает новый размер */
        return ptr;
    }

    /* иначе аллоцируем новый, копируем и освобождаем старый */
    void *newp = malloc(size);
    if (!newp) return NULL;
    memcpy(newp, ptr, block->size); /* копируем старый payload (block->size) */
    free(ptr);
    return newp;
}

/* show_alloc_mem: вывод зон и занятых блоков в формате subject */
static size_t print_zone(const char *name, t_zone *zone)
{
    size_t total = 0;
    for (t_zone *z = zone; z; z = z->next) {
        printf("%s : ", name);
        print_addr((void *)z);
        printf("\n");
        for (t_block *b = z->blocks; b; b = b->next) {
            if (!b->is_free) {
                void *start = (char *)b + sizeof(t_block);
                void *end = (char *)start + b->size;
                print_addr(start);
                printf(" - ");
                print_addr(end);
                printf(" : %zu bytes\n", b->size);
                total += b->size;
            }
        }
    }
    return total;
}

void show_alloc_mem(void)
{
    size_t total = 0;
    total += print_zone("TINY",  g_heap.tiny);
    total += print_zone("SMALL", g_heap.small);
    total += print_zone("LARGE", g_heap.large);
    printf("Total : %zu bytes\n", total);
}

/* ---------------- Test main (удали/закомментируй при интеграции в библиотеку) ----------------
   Простая демонстрация работы malloc/free/defrag/shrink.
*/
#ifdef MALLOC_TEST_MAIN
int main(void)
{
    printf("pagesize = %zu\n", pagesize());
    char *a = malloc(40);
    char *b = malloc(50);
    char *c = malloc(2000); /* large */
    char *d = malloc(30);

    strcpy(a, "hello a");
    strcpy(b, "hello b");

    show_alloc_mem();
    printf("---- free b ----\n");
    free(b);
    show_alloc_mem();
    printf("---- free a ---- (should defragment and maybe shrink)\n");
    free(a);
    show_alloc_mem();
    printf("---- free d ----\n");
    free(d);
    show_alloc_mem();
    printf("---- free c (large) ----\n");
    free(c);
    show_alloc_mem();
    return 0;
}
#endif
