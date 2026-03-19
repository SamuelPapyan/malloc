/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_block.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 12:09:17 by spapyan           #+#    #+#             */
/*   Updated: 2026/03/09 12:09:17 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

void split_block(t_block *b, size_t size) {
    if (b->size >= size + sizeof(t_block) + 16) {
        t_block *new_b = (t_block *)((char *)b + sizeof(t_block) + size);
        new_b->size = b->size - size - sizeof(t_block);
        new_b->free = 1;
        new_b->next = b->next;
        new_b->prev = b;
        if (b->next) b->next->prev = new_b;
        b->next = new_b;
        b->size = size;
    }
}