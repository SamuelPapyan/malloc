/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc_mem.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 16:20:49 by spapyan           #+#    #+#             */
/*   Updated: 2026/02/28 16:20:49 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

void print_hex_dump(void *addr, size_t len) {
    unsigned char *pc = (unsigned char*)addr;
    unsigned char buff[17];
    size_t i;

    for (i = 0; i < len; i++) {
        if ((i % 16) == 0) {
            if (i != 0) ft_putuendl(buff);
            ft_putstr("  ");
            ft_puthex(i);
            ft_putstr(" ");
        }
        // printf(" %02x", pc[i]);
        ft_putstr(" ");
        ft_putform(pc[i]);
        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) buff[i % 16] = '.';
        else buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }
    while ((i % 16) != 0) {
        ft_putstr("   ");
        i++;
    }
    ft_putstr("  ");
    ft_putuendl(buff);
}

void show_alloc_mem_ex() {
    pthread_mutex_lock(&g_lock);
    t_zone *z = g_zones;

    ft_putendl("=== EXTENDED MEMORY DUMP ===");
    while (z) {
        t_block *b = z->blocks;
        while (b) {
            if (!b->free) {
                // printf("Block %p | Size: %zu\n", (void*)b, b->size);
                ft_putstr("Block ");
                ft_putptr((void*)b);
                ft_putstr(" | Size: ");
                ft_putnbr(b->size);
                ft_putendl("");
                print_hex_dump((char *)b + sizeof(t_block), b->size);
                ft_putendl("");
            }
            b = b->next;
        }
        z = z->next;
    }
    pthread_mutex_unlock(&g_lock);
}

void show_alloc_mem() {
    pthread_mutex_lock(&g_lock);
    size_t total = 0;
    char *titles[] = {"TINY", "SMALL", "LARGE"};

    for (int i = 0; i < 3; i++) {
        ft_putstr(titles[i]);
        ft_putstr(" : ");
        ft_putptr((void *)g_zones);
        ft_putendl("");
        t_zone *z = g_zones;
        while (z) {
            if (z->type == (size_t)i) {
                t_block *b = z->blocks;
                while (b) {
                    if (!b->free) {
                        void *start = (char *)b + sizeof(t_block);
                        void *end = (char *)start + b->size;
                        ft_putptr(start);
                        ft_putstr(" - ");
                        ft_putptr(end);
                        ft_putstr(" : ");
                        ft_putnbr(b->size);
                        ft_putendl(" bytes");
                        total += b->size;
                    }
                    b = b->next;
                }
            }
            z = z->next;
        }
    }
    ft_putstr("Total : ");
    ft_putnbr(total);
    ft_putendl(" bytes");
    pthread_mutex_unlock(&g_lock);
}