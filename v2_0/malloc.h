/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 11:16:18 by spapyan           #+#    #+#             */
/*   Updated: 2026/03/09 11:16:18 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>

#define TINY_MAX 128
#define SMALL_MAX 1024

#define TINY_HEAP_SIZE ((TINY_MAX + sizeof(t_block)) * 128)
#define SMALL_HEAP_SIZE ((SMALL_MAX + sizeof(t_block)) * 128)

typedef struct s_block {
    size_t          size;
    struct s_block  *next;
    struct s_block  *prev;
    int             free;
} t_block;

typedef struct s_zone {
    size_t          type; // 0: TINY, 1: SMALL, 2: LARGE
    size_t          total_size;
    struct s_zone   *next;
    t_block         *blocks;
} t_zone;

extern pthread_mutex_t g_lock;
extern t_zone *g_zones;

void    ft_putchar(char c);
void    ft_putstr(char *str);
void    ft_putendl(char *str);
void    ft_putnbr(int nbr);
void	ft_puthex(int nbr);
void	ft_putuendl(unsigned char *str);
void	ft_putptr(void *ptr);
void	ft_putform(int nbr);

size_t align_size(size_t size);
size_t get_zone_size(size_t size, int type);

void split_block(t_block *b, size_t size);
t_zone *create_zone(size_t size, int type);

void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);

void show_alloc_mem();