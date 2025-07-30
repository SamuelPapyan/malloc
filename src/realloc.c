#include "malloc.h"

void    *realloc(void *ptr, size_t size) {
    if (!ptr)
        return malloc(size);
    if (size == 0) {
        free(ptr);
        return NULL;
    }

    t_block *block = PTR_TO_BLOCK(ptr);
    if (block->size >= size)
        return ptr;

    void *new_ptr = malloc(size);
    if (!new_ptr)
        return NULL;

    size_t copy_size = block->size < size ? block->size : size;
    ft_memcpy(new_ptr, ptr, copy_size);
    free(ptr);
    return new_ptr;
    // if (ptr == NULL)
    //     return malloc(size);

    // if (size == 0){
    //     free(ptr);
    //     return NULL;
    // }

    // size = ALIGN(size);

    // t_block *block = PTR_TO_BLOCK(ptr);

    // if (size <= block->size) {
    //     if (block->size - size >= sizeof(t_block) + MIN_REALLOC_DIFF)
    //         split_block(block, size);
    //     return ptr;
    // }

    // if (block->next && block->next->is_free &&
    //     block->size + sizeof(t_block) + block->next->size >= size) {
    //     block->size += sizeof(t_block) + block->next->size;
    //     block->next = block->next->next;
    //     if (block->next)
    //         block->next->prev = block;

    //     if (block->size - size >= sizeof(t_block) + MIN_REALLOC_DIFF)
    //         split_block(block, size);

    //     return ptr;
    // }

    // void    *new_ptr = malloc(size);
    // if (!new_ptr)
    //     return NULL;

    // memcpy(new_ptr, ptr, block->size);
    // free(ptr);
    // return new_ptr;
}