/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   should_unmap_zone.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/19 11:55:19 by spapyan           #+#    #+#             */
/*   Updated: 2026/03/19 11:55:19 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

int is_zone_empty(t_zone *zone) {
    t_block *b = zone->blocks;

    if (b && b->free && b->next == NULL && b->prev == NULL) {
        return 1;
    }
    return 0;
}

int should_unmap_zone(t_zone *z) {
    if (z->type == 2) {
        return 1;
    }

    t_zone *curr = g_zones;
    int empty_found = 0;

    while (curr) {
        if (curr->type == z->type && is_zone_empty(curr)) {
            empty_found++;
        }
        if (empty_found > 1) {
            return 1;
        }
        curr = curr->next;
    }
    return 0;
}