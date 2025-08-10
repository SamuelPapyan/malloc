/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 21:11:08 by spapyan           #+#    #+#             */
/*   Updated: 2025/08/09 21:11:08 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc2.h"

void	ft_bzero(void *s, size_t n)
{
	int	i;

	i = -1;
	if (n > 0)
	{
		while ((size_t)++i < n)
		{
			*(unsigned char *)(s + i) = '\0';
		}
	}
}

void	ft_putstr_fd(char const *s, int fd)
{
	if (!s)
		return ;
	while (*s)
		ft_putchar_fd(*(s++), fd);
}

void	ft_putchar_fd(char c, int fd)
{
	write(fd, &c, 1);
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

void	ft_putnbr_fd(int n, int fd)
{
	if (n < 0)
		ft_putchar_fd('-', fd);
	print_number(n, fd);
}