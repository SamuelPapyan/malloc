/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc2.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 20:16:56 by spapyan           #+#    #+#             */
/*   Updated: 2025/08/09 20:16:56 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_H
# define MALLOC_H

# include <stddef.h>
# include <sys/mman.h>
# include <unistd.h>
# include <stdint.h>

# define ALIGNMENT 16
# define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

# define TINY_MAX   64
# define SMALL_MAX  1024

# define MIN_BLOCKS_PER_ZONE 100

typedef struct s_block {
    size_t          size;
    int             is_free;
    struct s_block  *next;
} t_block;

typedef struct s_zone {
    size_t          size;
    struct s_zone   *next;
    t_block         *blocks;
} t_zone;

typedef struct s_heap {
    t_zone  *tiny;
    t_zone  *small;
    t_zone  *large;
} t_heap;

extern t_heap g_heap;

void    *malloc(size_t size);
void    free(void *ptr);
void    *realloc(void *ptr, size_t size);

t_block *find_free_block(t_zone *zone, size_t size);
t_block *find_block_from_ptr(void *ptr);
int     is_valid_pointer(void *ptr);

size_t  get_zone_size(size_t block_size);
t_zone  *create_zone(size_t block_size);
int     block_belongs_to_zone(t_zone *zone, t_block *block);
int     zone_all_free(t_zone *zone);
void    remove_zone(t_zone **list, t_zone *zone);

void    show_alloc_mem(void);

void    ft_puthexa_fd(uint64_t nb, int fd);
void	ft_bzero(void *s, size_t n);
void	ft_putstr_fd(char const *s, int fd);
void	ft_putchar_fd(char c, int fd);
void	*ft_memcpy(void *dest, const void *src, size_t n);
void	ft_putnbr_fd(int n, int fd);

#endif