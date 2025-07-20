#include "malloc.h"

t_block *g_block_list = NULL;
t_zone  *g_tiny = NULL;
t_zone  *g_small = NULL;
t_zone  *g_large = NULL;

void    *malloc(size_t size) {
    if (size == 0)
        return NULL;

    size = ALIGN(size);

    t_zone  **zone_list = get_zone(size);
    t_block *block = find_free_block(*zone_list, size);

    if (!block) {
        *zone_list = allocate_new_zone(*zone_list, size);
        if (!*zone_list) return NULL;
        block = (*zone_list)->blocks;
    }

    split_block(block, size);
    block->is_free = 0;
    return BLOCK_TO_PTR(block);
}