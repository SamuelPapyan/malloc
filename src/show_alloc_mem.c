#include <stdio.h>
#include "malloc.h"

static void print_block(t_block *block) {
    void *start = BLOCK_TO_PTR(block);
    void *end = (char *)start + block->size;
    ft_puthexa((uint64_t)start);
    ft_putstr_fd(" - ", 1);
    ft_puthexa((uint64_t)end);
    ft_putstr_fd(" | ", 1);
    ft_putnbr_fd(block->size, 1);
    ft_putstr_fd(" bytes\n", 1);
}

void show_blocks(t_zone *zone) {
    while (zone) {
        t_block *block = zone->blocks;
        while (block) {
            print_block(block);
            block = block->next;
        }
        zone = zone->next;
    }
}

static void    print_zone(char *zone_name, t_zone *zone) {
    ft_putstr_fd(zone_name, 1);
    ft_putstr_fd(" : ", 1);
    ft_puthexa((uint64_t)zone);
    ft_putchar_fd('\n', 1);
    show_blocks(zone);
}

static void sum_in_zone(int *sum, t_zone *zone) {
    while (zone) {
        t_block *block = zone->blocks;
        while (block) {
            *sum += block->size;
            block = block->next;
        }
        zone = zone->next;
    }
}

static void print_total(void) {
    int total;

    total = 0;
    sum_in_zone(&total, g_heap.tiny_zone);
    sum_in_zone(&total, g_heap.small_zone);
    sum_in_zone(&total, g_heap.large_zone);
    ft_putstr_fd("Total : ", 1);
    ft_putnbr_fd(total, 1);
    ft_putchar_fd('\n', 1);
}

void    show_alloc_mem(void) {
    print_zone("TINY", g_heap.tiny_zone);
    print_zone("SMALL", g_heap.small_zone);
    print_zone("LARGE", g_heap.large_zone);
    print_total();
}