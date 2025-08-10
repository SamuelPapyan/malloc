/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 20:28:10 by spapyan           #+#    #+#             */
/*   Updated: 2025/08/09 20:28:10 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc2.h"

t_heap  g_heap = {
    .tiny = NULL,
    .small = NULL,
    .large = NULL
};

void *malloc(size_t size) {
    if (size == 0)
        return NULL;

    size = ALIGN(size);

    // LARGE as a seperate zone
    if (size > SMALL_MAX) {
        size_t total_size = sizeof(t_zone) + sizeof(t_block) + size;
        size_t pagesize = (size_t)sysconf(_SC_PAGESIZE);
        total_size = ((total_size + pagesize - 1) / pagesize) * pagesize;

        t_zone *zone = mmap(NULL, total_size,
                            PROT_READ | PROT_WRITE,
                            MAP_PRIVATE | MAP_ANON,
                            -1, 0);
        if (zone == MAP_FAILED)
            return NULL;

        zone->size = total_size;
        zone->next = g_heap.large;
        g_heap.large = zone;

        t_block *block = (t_block *)((char *)zone + sizeof(t_zone));
        block->size = size;
        block->is_free = 0;
        block->next = NULL;

        return (char *)block + sizeof(t_block);
    }

    // Defining zone type
    t_zone **zone_list;
    size_t block_limit;
    if (size <= TINY_MAX) {
        zone_list = &g_heap.tiny;
        block_limit = TINY_MAX;
    } else {
        zone_list = &g_heap.small;
        block_limit = SMALL_MAX;
    }

    // Finding free block
    t_zone *zone = *zone_list;
    while (zone) {
        t_block *b = find_free_block(zone, size);
        if (b) {
            b->is_free = 0;
            return (char *)b + sizeof(t_block);
        }
        zone = zone->next;
    }

    // There is no block - create new zone
    t_zone *new_zone = create_zone(block_limit);
    if (!new_zone)
        return NULL;

    new_zone->next = *zone_list;
    *zone_list = new_zone;

    // The zone's first block
    new_zone->blocks->is_free = 0;
    new_zone->blocks->size = size;

    return (char *)new_zone->blocks + sizeof(t_block);
}