/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   page.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 16:28:54 by spapyan           #+#    #+#             */
/*   Updated: 2026/02/28 16:28:54 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

size_t get_zone_size(size_t size, int type) {
    size_t page = sysconf(_SC_PAGESIZE);
    size_t total;

    if (type == 2) total = size + sizeof(t_zone) + sizeof(t_block);
    else if (type == 1) total = SMALL_HEAP_SIZE + sizeof(t_zone);
    else total = TINY_HEAP_SIZE + sizeof(t_zone);

    return (total + page - 1) & ~(page - 1);
}

size_t align_size(size_t size) {
    return (size + 15) & ~15;
}

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	char		*dstc;
	const char	*srcc;

	if ((dest == src) || n == 0)
		return (dest);
	if (!dest && !src)
		return (0);
	dstc = (char *)dest;
	srcc = (const char *)src;
	while (n--)
		dstc[n] = srcc[n];
	return (dest);
}
