/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 13:44:53 by spapyan           #+#    #+#             */
/*   Updated: 2026/03/03 13:44:53 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

static void    ft_putchar_fd(char c, int fd) {
    write(fd, &c, 1);
}

static void	ft_putstr_fd(char *s, int fd)
{
	if (!s)
		return ;
	while (*s)
		ft_putchar_fd(*(s++), fd);
}

static void	ft_putendl_fd(char *s, int fd)
{
	if (!s)
		return ;
	ft_putstr_fd(s, fd);
	ft_putchar_fd('\n', fd);
}

static void	print_number(int nbr, int fd)
{
	char	num;

	if (nbr / 10 != 0)
		print_number(nbr / 10, fd);
	num = nbr % 10;
	if (num < 0)
		num = -num;
	ft_putchar_fd(num + '0', fd);
}
 
static void	ft_putnbr_fd(int n, int fd)
{
	if (n < 0)
		ft_putchar_fd('-', fd);
	print_number(n, fd);
}

static void print_hex_number(int nbr, int fd) {
	char*	symbols = "0123456789ABCDEF";
	int	num;

	if (nbr / 16 != 0) {
		print_hex_number(nbr / 16, fd);
	}
	num = nbr % 16;
	ft_putchar_fd(symbols[num], fd);
}

static void	ft_puthex_fd(int n, int fd)
{
	if (n < 0)
		ft_putchar_fd('-', fd);
	ft_putstr_fd("0x", fd);
	print_hex_number(n, fd);
}

void    ft_putchar(char c) {
    ft_putchar_fd(c, 1);
}

void    ft_putstr(char *str) {
    ft_putstr_fd(str, 1);
}

void    ft_putendl(char *str) {
    ft_putendl_fd(str, 1);
}

void    ft_putnbr(int nbr) {
    ft_putnbr_fd(nbr, 1);
}

void	ft_puthex(int nbr) {
	ft_puthex_fd(nbr, 1);
}

void	ft_putuendl(unsigned char *str) {
	if (!str)
		return ;
	while (*str)
		ft_putchar(*(str++));
	ft_putchar('\n');

}

void	ft_putptr(void *ptr) {
	ft_putstr("0x");
	unsigned long addr = (unsigned long)ptr;

	int nibbles = sizeof(void*) * 2;
	char buffer[16];
	const char *symbols = "0123456789ABCDEF";

	for (int i = nibbles - 1; i >= 0; i--) {
		buffer[i] = symbols[addr & 0xF];
		addr >>= 4;
	}
	write(1, buffer, nibbles);
}

void	ft_putform(int nbr) {
	if (nbr > 15) ft_putchar('0');
	ft_puthex(nbr);
}