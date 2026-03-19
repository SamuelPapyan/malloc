/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_valid_pointer.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 16:44:15 by spapyan           #+#    #+#             */
/*   Updated: 2025/08/10 16:44:15 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc2.h"

int is_valid_pointer(void *ptr) {
    t_zone *zone;
    t_block *block;

    if (!ptr)
        return 0;

    zone = g_heap.tiny;
    while (zone) {
        block = zone->blocks;
        while (block) {
            if (((void *)((char *)block + sizeof(t_block))) == ptr && block->is_free == 0)
                return 1;
            block = block->next;
        }
        zone = zone->next;
    }

    zone = g_heap.small;
    while (zone) {
        block = zone->blocks;
        while (block) {
            if (((void *)((char *)block + sizeof(t_block))) == ptr && block->is_free == 0)
                return 1;
            block = block->next;
        }
        zone = zone->next;
    }

    zone = g_heap.large;
    while (zone) {
        block = zone->blocks;
        while (block) {
            if (((void *)((char *)block + sizeof(t_block))) == ptr && block->is_free == 0)
                return 1;
            block = block->next;
        }
        zone = zone->next;
    }

    return 0;
}