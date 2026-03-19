/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_valid_pointer.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 18:40:39 by spapyan           #+#    #+#             */
/*   Updated: 2025/08/08 18:40:39 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

int is_valid_pointer(void *ptr) {
    t_zone *zone;
    t_block *block;

    if (!ptr)
        return 0;

    // Проверка в TINY
    zone = g_heap.tiny_zone;
    while (zone) {
        block = zone->blocks;
        while (block) {
            if (BLOCK_TO_PTR(block) == ptr && block->is_free == 0)
                return 1;
            block = block->next;
        }
        zone = zone->next;
    }

    // Проверка в SMALL
    zone = g_heap.small_zone;
    while (zone) {
        block = zone->blocks;
        while (block) {
            if (BLOCK_TO_PTR(block) == ptr && block->is_free == 0)
                return 1;
            block = block->next;
        }
        zone = zone->next;
    }

    // Проверка в LARGE
    zone = g_heap.large_zone;
    while (zone) {
        block = zone->blocks;
        while (block) {
            if (BLOCK_TO_PTR(block) == ptr && block->is_free == 0)
                return 1;
            block = block->next;
        }
        zone = zone->next;
    }

    // Не нашли подходящего блока
    return 0;
}