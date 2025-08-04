/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   zone.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 20:50:37 by spapyan           #+#    #+#             */
/*   Updated: 2025/08/04 20:50:37 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

static size_t  zone_alloc_size(size_t size) {
    size_t  pagesize = getpagesize();
    if (size < TINY_LIMIT)
        return pagesize * 8;
    else if (size <= SMALL_LIMIT)
        return pagesize * 32;
    else
        return size + sizeof(t_zone) + sizeof(t_block);
}

t_zone  **get_zone(size_t size) {
    if (size <= TINY_LIMIT)
        return &(g_heap.tiny_zone);
    else if (size <= SMALL_LIMIT)
        return &(g_heap.small_zone);
    else
        return &(g_heap.large_zone);
}

t_zone  *allocate_new_zone(t_zone *prev, size_t size) {
    size_t  alloc_size = zone_alloc_size(size);

    void    *ptr = mmap(NULL, alloc_size,
                        PROT_READ | PROT_WRITE,
                        MAP_ANON | MAP_PRIVATE, -1 ,0);

    if (ptr == MAP_FAILED)
        return NULL;

    t_zone *zone = (t_zone *)ptr;
    zone->size = alloc_size;
    zone->start = ptr;
    zone->next = NULL;

    if (prev)
        prev->next = zone;

    t_block *block = (t_block *)(zone + 1);
    block->size = alloc_size - sizeof(t_zone) - sizeof(t_block);
    block->is_free = 1;
    block->next = NULL;
    block->prev = NULL;
    
    zone->blocks = block;
    return zone;
} 