#include "malloc.h"

void    free(void *ptr) {
    if (!ptr) return;

    t_block *block = PTR_TO_BLOCK(ptr);
    block->is_free = 1;

    coalesce_blocks(block);
}