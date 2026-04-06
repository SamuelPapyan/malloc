/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_block.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/06 17:03:05 by spapyan           #+#    #+#             */
/*   Updated: 2026/04/06 17:03:05 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

t_block *find_block(void *ptr, t_zone **out_zone, t_zone ***out_prev_z) {
    t_zone **prev_z = &g_zones;
    t_zone *z = g_zones;
    while (z) {
        t_block *b = z->blocks;
        while (b) {
            if ((void *)((char *)b + sizeof(t_block)) == ptr) {
                if (out_zone) *out_zone = z;
                if (out_prev_z) *out_prev_z = prev_z;
                return b;
            }
            b = b->next;
        }
        prev_z = &(z->next);
        z = z->next;
    }
    return NULL;
}