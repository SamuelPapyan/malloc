/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 16:14:54 by spapyan           #+#    #+#             */
/*   Updated: 2026/02/28 16:14:54 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

void *malloc(size_t size) {
    if (size <= 0) return NULL;
    size = align_size(size);
    int type = (size <= TINY_MAX) ? 0 : (size <= SMALL_MAX ? 1 : 2);

    pthread_mutex_lock(&g_lock);

    // 1. Поиск в существующих TINY/SMALL зонах
    if (type < 2) {
        t_zone *z = g_zones;
        while (z) {
            if (z->type == (size_t)type) {
                t_block *b = z->blocks;
                while (b) {
                    if (b->free && b->size >= size) {
                        split_block(b, size);
                        b->free = 0;
                        pthread_mutex_unlock(&g_lock);
                        return (void *)((char *)b + sizeof(t_block));
                    }
                    b = b->next;
                }
            }
            z = z->next;
        }
    }

    // 2. Если места нет или это LARGE — создаем новую зону
    t_zone *new_z = create_zone(size, type);
    if (!new_z) {
        pthread_mutex_unlock(&g_lock);
        return NULL;
    }

    // Вставляем новую зону в начало списка
    new_z->next = g_zones;
    g_zones = new_z;

    t_block *first_b = new_z->blocks;
    if (type < 2) {
        split_block(first_b, size);
    } else {
        // Для LARGE отдаем всё пространство зоны за вычетом заголовков
        first_b->size = new_z->total_size - sizeof(t_zone) - sizeof(t_block);
    }
    
    first_b->free = 0;
    pthread_mutex_unlock(&g_lock);
    return (void *)((char *)first_b + sizeof(t_block));
}