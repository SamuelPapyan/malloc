/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   full_project_2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 11:13:20 by spapyan           #+#    #+#             */
/*   Updated: 2026/03/09 11:13:20 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
t_zone *g_zones = NULL;

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

// --- Вспомогательные функции ---

size_t align_size(size_t size) {
    return (size + 15) & ~15;
}

size_t get_zone_size(size_t size, int type) {
    size_t page = sysconf(_SC_PAGESIZE);
    size_t total;

    if (type == 2) total = size + sizeof(t_zone) + sizeof(t_block);
    else if (type == 1) total = SMALL_HEAP_SIZE + sizeof(t_zone);
    else total = TINY_HEAP_SIZE + sizeof(t_zone);

    return (total + page - 1) & ~(page - 1);
}

// Разбиение блока на два, если места слишком много
void split_block(t_block *b, size_t size) {
    if (b->size >= size + sizeof(t_block) + 16) {
        t_block *new_b = (t_block *)((char *)b + sizeof(t_block) + size);
        new_b->size = b->size - size - sizeof(t_block);
        new_b->free = 1;
        new_b->next = b->next;
        new_b->prev = b;
        if (b->next) b->next->prev = new_b;
        b->next = new_b;
        b->size = size;
    }
}

t_zone *create_zone(size_t size, int type) {
    size_t total_needed = get_zone_size(size, type);
    t_zone *zone = mmap(NULL, total_needed, PROT_READ | PROT_WRITE, 
                        MAP_ANON | MAP_PRIVATE, -1, 0);
    if (zone == MAP_FAILED) return NULL;

    zone->type = type;
    zone->total_size = total_needed;
    zone->next = NULL;
    zone->blocks = (t_block *)((char *)zone + sizeof(t_zone));
    zone->blocks->size = total_needed - sizeof(t_zone) - sizeof(t_block);
    zone->blocks->free = 1;
    zone->blocks->next = NULL;
    zone->blocks->prev = NULL;
    return zone;
}

// --- Основные функции библиотеки ---

void *malloc(size_t size) {
    if (size <= 0) return NULL;
    size = align_size(size);
    int type = (size <= TINY_MAX) ? 0 : (size <= SMALL_MAX ? 1 : 2);

    pthread_mutex_lock(&g_lock);

    // 1. Поиск в существующих TINY/SMALL зонах
    if (type < 2) {
        t_zone *z = g_zones;
        while (z) {
            if (z->type == (size_t)type) {
                t_block *b = z->blocks;
                while (b) {
                    if (b->free && b->size >= size) {
                        split_block(b, size);
                        b->free = 0;
                        pthread_mutex_unlock(&g_lock);
                        return (void *)((char *)b + sizeof(t_block));
                    }
                    b = b->next;
                }
            }
            z = z->next;
        }
    }

    // 2. Если места нет или это LARGE — создаем новую зону
    t_zone *new_z = create_zone(size, type);
    if (!new_z) {
        pthread_mutex_unlock(&g_lock);
        return NULL;
    }

    // Вставляем новую зону в начало списка
    new_z->next = g_zones;
    g_zones = new_z;

    t_block *first_b = new_z->blocks;
    if (type < 2) {
        split_block(first_b, size);
    } else {
        // Для LARGE отдаем всё пространство зоны за вычетом заголовков
        first_b->size = new_z->total_size - sizeof(t_zone) - sizeof(t_block);
    }
    
    first_b->free = 0;
    pthread_mutex_unlock(&g_lock);
    return (void *)((char *)first_b + sizeof(t_block));
}

void free(void *ptr) {
    if (!ptr) return;
    pthread_mutex_lock(&g_lock);

    t_zone **prev_z = &g_zones;
    t_zone *z = g_zones;

    while (z) {
        t_block *b = z->blocks;
        while (b) {
            if ((void *)((char *)b + sizeof(t_block)) == ptr) {
                b->free = 1;

                // Слияние (Coalescing)
                if (b->next && b->next->free) {
                    b->size += sizeof(t_block) + b->next->size;
                    b->next = b->next->next;
                    if (b->next) b->next->prev = b;
                }
                if (b->prev && b->prev->free) {
                    b->prev->size += sizeof(t_block) + b->size;
                    b->prev->next = b->next;
                    if (b->next) b->next->prev = b->prev;
                }

                // Удаление пустых LARGE зон
                if (z->type == 2) {
                    *prev_z = z->next;
                    munmap(z, z->total_size);
                }
                pthread_mutex_unlock(&g_lock);
                return;
            }
            b = b->next;
        }
        prev_z = &(z->next);
        z = z->next;
    }
    pthread_mutex_unlock(&g_lock);
}

void *realloc(void *ptr, size_t size) {
    if (!ptr) return malloc(size);
    if (size == 0) {
        free(ptr);
        return NULL;
    }

    t_block *b = (t_block *)((char *)ptr - sizeof(t_block));
    size_t aligned_new = align_size(size);

    // Если текущий блок уже подходит по размеру
    if (b->size >= aligned_new) return ptr;

    void *new_ptr = malloc(size);
    if (new_ptr) {
        memcpy(new_ptr, ptr, b->size);
        free(ptr);
    }
    return new_ptr;
}

void show_alloc_mem() {
    pthread_mutex_lock(&g_lock);
    size_t total = 0;
    char *titles[] = {"TINY", "SMALL", "LARGE"};

    for (int i = 0; i < 3; i++) {
        // printf("%s : %p\n", titles[i], (void *)g_zones);
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
                        // printf("%p - %p : %zu bytes\n", start, end, b->size);
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
    // printf("Total : %zu bytes\n", total);
    ft_putstr("Total : ");
    ft_putnbr(total);
    ft_putendl(" bytes");
    pthread_mutex_unlock(&g_lock);
}