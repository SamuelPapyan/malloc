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
    void *ptr1 = malloc(100);
    if (ptr1)
        memset(ptr1, 'A', 10);
    show_alloc_mem();
    free(ptr1);
    ft_putstr_fd("=== show_alloc_mem() after free ===\n", 1);
    show_alloc_mem();

    return 0;
}
