/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 20:36:00 by spapyan           #+#    #+#             */
/*   Updated: 2025/08/09 20:36:00 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc2.h"

void free(void *ptr) {
    if (!ptr)
        return;

    t_block *block = (t_block *)((char *)ptr - sizeof(t_block));

    // Checking LARGE zone — free immediately
    t_zone **large_list = &g_heap.large;
    t_zone *prev = NULL;
    for (t_zone *zone = *large_list; zone; prev = zone, zone = zone->next) {
        if (block_belongs_to_zone(zone, block)) {
            if (prev)
                prev->next = zone->next;
            else
                *large_list = zone->next;
            munmap(zone, zone->size);
            return;
        }
    }

    // Looking in TINY/SMALL
    t_zone **lists[2] = { &g_heap.tiny, &g_heap.small };
    for (int i = 0; i < 2; i++) {
        for (t_zone *zone = *lists[i]; zone; zone = zone->next) {
            if (block_belongs_to_zone(zone, block)) {
                block->is_free = 1;
                if (zone_all_free(zone))
                    remove_zone(lists[i], zone);
                return;
            }
        }
    }
}