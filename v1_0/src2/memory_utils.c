/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 20:27:26 by spapyan           #+#    #+#             */
/*   Updated: 2025/08/09 20:27:26 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc2.h"

t_block *find_free_block(t_zone *zone, size_t size) {
    for (t_block *b = zone->blocks; b != NULL; b = b->next) {
        if (b->is_free && b->size >= size)
            return b;
    }
    return NULL;
}

t_block *find_block_from_ptr(void *ptr) {
    if (!ptr)
        return NULL;
    return (t_block *)((char *)ptr - sizeof(t_block));
}