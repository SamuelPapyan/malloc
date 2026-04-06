/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   zones.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 16:18:47 by spapyan           #+#    #+#             */
/*   Updated: 2026/02/28 16:18:47 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

t_zone *create_zone(size_t size, int type) {
    size_t total_needed = get_zone_size(size, type);
    t_zone *zone = mmap(NULL, total_needed, PROT_READ | PROT_WRITE, 
                        MAP_ANON | MAP_PRIVATE, -1, 0);
    if (zone == MAP_FAILED) {
        errno = ENOMEM;
        return NULL;
    }

    zone->type = type;
    zone->total_size = total_needed;
    zone->next = NULL;

    zone->blocks = (t_block *)((char *)zone + sizeof(t_zone));

    zone->blocks->size = total_needed - sizeof(t_zone) - sizeof(t_block);
    zone->blocks->free = 1;
    zone->blocks->next = NULL;
    zone->blocks->prev = NULL;

    return zone;
}