/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 16:18:44 by spapyan           #+#    #+#             */
/*   Updated: 2026/02/28 16:18:44 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

void free(void *ptr) {
    if (!ptr) return;
    pthread_mutex_lock(&g_lock);

    t_zone **prev_z = &g_zones;
    t_zone *z = g_zones;

    while (z) {
        t_block *b = z->blocks;
        while (b) {
            if ((void *)((char *)b + sizeof(t_block)) == ptr) {
                if (b->free) {
                    pthread_mutex_unlock(&g_lock);
                    return;
                }
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
                if (b->prev == NULL)
                    z->blocks = b;

                if (is_zone_empty(z) && should_unmap_zone(z)) {
                    t_zone *to_unmap = z;
                    *prev_z = z->next;
                    munmap(to_unmap, to_unmap->total_size);
                    pthread_mutex_unlock(&g_lock);
                    return;
                }
                pthread_mutex_unlock(&g_lock);
                return;
            }
            b = b->next;
        }
        prev_z = &(z->next);
        z = z->next;
    }
    pthread_mutex_unlock(&g_lock);
}