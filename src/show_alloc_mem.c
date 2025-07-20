#include <stdio.h>
#include "malloc.h"

void show_blocks(t_zone *zone) {
    while (zone) {
        t_block *block = zone->blocks;
        while (block) {
            void *start = BLOCK_TO_PTR(block);
            void *end = (char *)start + block->size;
            printf("[%s] : %p - %p | %zu bytes\n",
                   block->is_free ? "FREE" : "USED",
                   start, end, block->size);
            block = block->next;
        }
        zone = zone->next;
    }
}

void show_alloc_mem(void) {
    printf("=== Memory Map ===\n");
    printf("TINY : %p\n", g_tiny);   show_blocks(g_tiny);
    printf("SMALL : %p\n", g_small); show_blocks(g_small);
    printf("LARGE : %p\n", g_large); show_blocks(g_large);
}