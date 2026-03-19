/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 20:50:44 by spapyan           #+#    #+#             */
/*   Updated: 2025/08/04 20:50:44 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include "include/malloc.h"

int main(void) {
    ft_putstr_fd("=== malloc(100) ===\n", 1);
    void *ptr1 = malloc(64);
    void *ptr2 = malloc(512);
    void *ptr3 = malloc(1024);
    void *ptr4 = malloc(1025);
    void *ptr5 = malloc(1025);
    ft_putstr_fd(ptr1, 1);
    ft_putchar_fd('\n', 1);
    show_alloc_mem();
    free(ptr1);
    void *free_buff = realloc(ptr5, 0);
    (void)free_buff;
    ft_putstr_fd("=== show_alloc_mem() after free ===\n", 1);
    show_alloc_mem();
    free(ptr2);
    free(ptr3);
    free(ptr4);
    return 0;
}
