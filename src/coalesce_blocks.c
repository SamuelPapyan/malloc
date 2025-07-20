#include "malloc.h"

void    coalesce_blocks(t_block *block) {
    if (block->next && block->next->is_free) {
        block->size += sizeof(t_block) + block->next->size;
        block->next = block->next->next;
        if (block->next)
            block->next->prev = block;
    }

    if (block->prev && block->prev->is_free) {
        // block->prev->size += sizeof(t_block) + block->size;
        // block->prev->next = block->next;
        // if (block->next)
        //     block->next->prev = block->prev;
        block = block->prev;
        coalesce_blocks(block);
    }
}