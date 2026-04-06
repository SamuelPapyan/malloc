/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   internal_free.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/06 17:05:33 by spapyan           #+#    #+#             */
/*   Updated: 2026/04/06 17:05:33 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

void    internal_free(void *ptr) {
    t_zone *z;
    t_zone **prev_z;
    t_block *b = find_block(ptr, &z, &prev_z);
    
    if (!b || b->free) return;

    b->free = 1;

    if (b->next && b->next->free) {
        b->size += sizeof(t_block) + b->next->size;
        b->next = b->next->next;
        if (b->next) b->next->prev = b;
    }

    if (b->prev && b->prev->free) {
        t_block *prev_b = b->prev;
        prev_b->size += sizeof(t_block) + b->size;
        prev_b->next = b->next;
        if (b->next) b->next->prev = prev_b;
        b = prev_b;
    }

    if (b->prev == NULL) {
        z->blocks = b;
    }

    if (is_zone_empty(z) && should_unmap_zone(z)) {
        *prev_z = z->next;
        munmap(z, z->total_size);
    }
}