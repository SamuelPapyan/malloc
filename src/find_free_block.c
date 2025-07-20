#include "malloc.h"

t_block *find_free_block(t_zone *zone, size_t size) {
    while (zone) {
        t_block *block = zone->blocks;
        while (block) {
            if (block->is_free && block->size >= size)
                return block;
            block = block->next;
        }
        zone = zone->next;
    }
    return NULL;
}

t_block *find_best_fit_block(size_t size) {
    t_block *current = g_block_list;
    t_block *best = NULL;

    while (current) {
        if (current->is_free && current->size >= size) {
            if (!best || current->size < best->size) {
                best = current;
            }
        }
        current = current->next;
    }
    return best;
}