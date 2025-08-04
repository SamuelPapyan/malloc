/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_free_block.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 20:49:58 by spapyan           #+#    #+#             */
/*   Updated: 2025/08/04 20:49:58 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

t_block *find_free_block(t_zone *zone, size_t size) {
    while (zone) {
        t_block *block = zone->blocks;
        while (block) {
            if (block->is_free && block->size >= size)
                return block;
            block = block->next;
        }
        zone = zone->next;
    }
    return NULL;
}