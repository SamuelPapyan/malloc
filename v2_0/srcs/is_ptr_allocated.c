/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_ptr_allocated.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/06 11:47:33 by spapyan           #+#    #+#             */
/*   Updated: 2026/04/06 11:47:33 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

int is_ptr_allocated(void *ptr) {
    t_zone *z = g_zones;
    while (z) {
        t_block *b = z->blocks;
        while (b) {
            if ((void *)((char *)b + sizeof(t_block)) == ptr) {
                return (b->free == 0); // Возвращаем истину, если блок занят
            }
            b = b->next;
        }
        z = z->next;
    }
    return 0;
}