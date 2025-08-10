/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc_mem.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 21:00:25 by spapyan           #+#    #+#             */
/*   Updated: 2025/08/09 21:00:25 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc2.h"

static void print_addr(void *addr) {
    if (addr)
        ft_puthexa_fd((uint64_t)addr, 1);
    else
        ft_putstr_fd("0x0", 1);
}

static void print_block(t_block *block) {
    void *start = (char *)block + sizeof(t_block);
    void *end   = (char *)start + block->size;
    print_addr(start);
    ft_putstr_fd(" - ", 1);
    print_addr(end);
    ft_putstr_fd(" : ", 1);
    ft_putnbr_fd(block->size, 1);
    ft_putstr_fd(" bytes\n", 1);
}

static size_t print_zone(const char *name, t_zone *zone) {
    size_t total = 0;
    while (zone) {
        ft_putstr_fd(name, 1);
        ft_putstr_fd(" : ", 1);
        print_addr(zone);
        ft_putchar_fd('\n', 1);

        for (t_block *b = zone->blocks; b; b = b->next) {
            if (!b->is_free) {
                print_block(b);
                total += b->size;
            }
        }
        zone = zone->next;
    }
    return total;
}

void show_alloc_mem(void) {
    size_t total = 0;

    total += print_zone("TINY",  g_heap.tiny);
    total += print_zone("SMALL", g_heap.small);
    total += print_zone("LARGE", g_heap.large);

    ft_putstr_fd("Total : ", 1);
    ft_putnbr_fd(total, 1);
    ft_putstr_fd(" bytes\n", 1);
}