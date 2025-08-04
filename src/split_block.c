/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_block.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 20:50:34 by spapyan           #+#    #+#             */
/*   Updated: 2025/08/04 20:50:34 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

void    split_block(t_block *block, size_t size) {
    if (block->size <= size + sizeof(t_block) + MIN_SPLIT_SIZE)
        return;

    t_block *new_block = (t_block *)((char *)BLOCK_TO_PTR(block) + size);
    new_block->size = block->size - size - sizeof(t_block);
    new_block->is_free = 1;
    new_block->next = block->next;
    new_block->prev = block;

    if (new_block->next)
        new_block->next->prev = new_block;

    block->size = size;
    block->next = new_block;
}