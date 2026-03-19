/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_memory.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 20:50:28 by spapyan           #+#    #+#             */
/*   Updated: 2025/08/04 20:50:28 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

t_block *request_memory(size_t size) {
    size_t  total_size = sizeof(t_block) + ALIGN(size);

    if (total_size < MIN_ZONE_SIZE)
        total_size= MIN_ZONE_SIZE;

    void *mem = mmap(NULL, total_size,
                    PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANON,
                    -1, 0);

    if (mem == MAP_FAILED)
        return NULL;

    t_block *block = (t_block *)mem;
    block->size = total_size - sizeof(t_block);
    block->is_free = 0;
    block->next = NULL;
    block->prev = NULL;

    return block;
}