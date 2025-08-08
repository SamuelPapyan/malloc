/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 20:50:01 by spapyan           #+#    #+#             */
/*   Updated: 2025/08/04 20:50:01 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

void    free(void *ptr) {
    if (!ptr) return;

    if (!is_valid_pointer(ptr)) {
        ft_putstr_fd("Error: invalid free(%p)\n", 2);
        ft_puthexa((uint64_t)ptr);
    }

    t_block *block = PTR_TO_BLOCK(ptr);
    block->is_free = 1;
    coalesce_blocks(block);
}