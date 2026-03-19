/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coalesce_blocks.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 20:49:54 by spapyan           #+#    #+#             */
/*   Updated: 2025/08/04 20:49:54 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

void    coalesce_blocks(t_block *block) {
    if (block->next && block->next->is_free) {
        block->size += sizeof(t_block) + block->next->size;
        block->next = block->next->next;
        if (block->next)
            block->next->prev = block;
    }

    if (block->prev && block->prev->is_free) {
        // block->prev->size += sizeof(t_block) + block->size;
        // block->prev->next = block->next;
        // if (block->next)
        //     block->next->prev = block->prev;
        block = block->prev;
        coalesce_blocks(block);
    }
}