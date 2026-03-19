/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   zone.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 20:25:13 by spapyan           #+#    #+#             */
/*   Updated: 2025/08/09 20:25:13 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc2.h"

size_t  get_zone_size(size_t block_size) {
    size_t pagesize = (size_t)sysconf(_SC_PAGESIZE);
    size_t meta_block = sizeof(t_block);
    size_t total_block_size = meta_block + block_size;
    size_t zone_size = total_block_size * MIN_BLOCKS_PER_ZONE + sizeof(t_zone);
    return ((zone_size + pagesize - 1) / pagesize) * pagesize;
}

t_zone  *create_zone(size_t block_size) {
    size_t zone_size = get_zone_size(block_size);
    t_zone *zone = mmap(NULL, zone_size,
                        PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANON,
                        -1, 0);
    if (zone == MAP_FAILED)
        return NULL;
    zone->size = zone_size;
    zone->next = NULL;
    zone->blocks = (t_block *)((char *)zone + sizeof(t_zone));

    // Initiating the first block
    zone->blocks->size = block_size;
    zone->blocks->is_free = 1;
    zone->blocks->next = NULL;
    return zone;
}

int block_belongs_to_zone(t_zone *zone, t_block *block) {
    char *zone_start = (char *)zone;
    char *zone_end = zone_start + zone->size;
    return ((char *)block >= zone_start && (char *)block < zone_end);
}

int zone_all_free(t_zone *zone) {
    for (t_block *b = zone->blocks; b; b = b->next) {
        if (!b->is_free)
            return 0;
    }
    return 1;
}

void    remove_zone(t_zone **list, t_zone *zone) {
    t_zone *prev = NULL;
    for (t_zone *z = *list; z; prev = z, z = z->next) {
        if (z == zone) {
            if (prev)
                prev->next = z->next;
            else
                *list = z->next;
            munmap(z, z->size);
            return;
        }
    }
}