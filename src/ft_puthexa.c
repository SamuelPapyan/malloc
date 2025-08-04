/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_puthexa.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 20:50:04 by spapyan           #+#    #+#             */
/*   Updated: 2025/08/04 20:50:04 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

void    ft_puthexa(uint64_t nb) {
    if (nb == 0) {
        ft_putstr_fd("0x0", 1);
        return;
    }
    char    hex_revert[16];
    int     hex[16];
    int     i;

    ft_bzero(hex_revert, 16);
    ft_bzero(hex, 16);
    i = 0;
    while (nb) {
        hex[i] = nb % 16;
        if (hex[i] >= 10)
            hex_revert[i] = hex[i] + 'A' - 10;
        else
            hex_revert[i] = hex[i] + '0';
        nb /= 16;
        i++;
    }
    ft_putstr_fd("0x", 1);
    while (i >= 0) {
        ft_putchar_fd(hex_revert[i], 1);
        i--;
    }
}