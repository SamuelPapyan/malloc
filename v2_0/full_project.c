/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   full_project.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 11:14:16 by spapyan           #+#    #+#             */
/*   Updated: 2026/03/09 11:14:16 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <pthread.h>
#include <string.h>

#define TINY_MAX 128
#define SMALL_MAX 1024

#define TINY_HEAP_SIZE ( (TINY_MAX + sizeof(t_block)) * 128 )
#define SMALL_HEAP_SIZE ( (SMALL_MAX + sizeof(t_block)) * 128 )


typedef struct s_block {
    size_t          size;    // Size of the usable data
    struct s_block  *next;   // Pointer to the next block in the zone
    struct s_block  *prev;   // Pointer to the previous block in the zone
    int             free;    // Boolean flag
} t_block;

typedef struct s_zone {
    size_t          type; // 0 for TINY, 1 for SMALL, 2 for LARGE
    size_t          total_size;
    struct s_zone   *next;
    t_block         *blocks;
} t_zone;

pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
t_zone *g_zones = NULL;

size_t  get_page_size();
size_t  align_size(size_t size);

t_zone  *create_zone(size_t size, int type);
size_t  get_zone_size(size_t size, int type);

void    *malloc(size_t size);
void    free(void *ptr);
void    *realloc(void *ptr, size_t size);
void    show_alloc_mem();

void    ft_putchar(char c);
void    ft_putstr(char *str);
void    ft_putendl(char *str);
void    ft_putnbr(int nbr);
void	ft_puthex(int nbr);
void	ft_putptr(void *ptr);
void	ft_putform(int nbr);
void	ft_putuendl(unsigned char *str);

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

// static size_t ft_strlen(char *str) {
// 	size_t	i = 0;
// 	while (str[i] != '\0')
// 		i++;
// 	return i;
// }

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

t_zone *create_zone(size_t size, int type) {
    size_t total_needed = get_zone_size(size, type);
    
    t_zone *zone = mmap(NULL, total_needed, PROT_READ | PROT_WRITE, 
                        MAP_ANON | MAP_PRIVATE, -1, 0);
    if (zone == MAP_FAILED) return NULL;

    zone->type = type;
    zone->total_size = total_needed;
    zone->next = NULL;
    
    // Set up the first block within the zone
    zone->blocks = (t_block *)((char *)zone + sizeof(t_zone));
    zone->blocks->size = total_needed - sizeof(t_zone) - sizeof(t_block);
    zone->blocks->next = NULL;
    zone->blocks->prev = NULL;
    zone->blocks->free = 1;

    return zone;
}

size_t get_zone_size(size_t size, int type) {
    size_t page = sysconf(_SC_PAGESIZE);
    size_t zone_size;

    if (type == 2) // LARGE
        zone_size = size + sizeof(t_zone) + sizeof(t_block);
    else if (type == 1) // SMALL
        zone_size = SMALL_HEAP_SIZE + sizeof(t_zone);
    else // TINY
        zone_size = TINY_HEAP_SIZE + sizeof(t_zone);

    // Round up to the nearest multiple of page size
    return (zone_size + page - 1) & ~(page - 1);
}


size_t get_page_size() {
    return sysconf(_SC_PAGESIZE);
}

size_t align_size(size_t size) {
    return (size + 15) & ~15; // 16-byte alignment
}

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

void *realloc(void *ptr, size_t size) {
    if (!ptr) return malloc(size);
    if (size == 0) {
        free(ptr);
        return NULL;
    }

    t_block *b = (t_block *)((char *)ptr - sizeof(t_block));
    if (b->size >= size) return ptr;

    void *new_ptr = malloc(size);
    if (new_ptr) {
        memcpy(new_ptr, ptr, b->size);
        free(ptr);
    }
    return new_ptr;
}

void *malloc(size_t size) {
    if (size <= 0) return NULL;
    size = align_size(size);
    
    pthread_mutex_lock(&g_lock);

    int type = (size <= TINY_MAX) ? 0 : (size <= SMALL_MAX ? 1 : 2);
    t_zone **curr = &g_zones;

    if (type < 2) {
        while (*curr) {
            if ((*curr)->type == (size_t)type) {
                t_block *b = (*curr)->blocks;
                while (b) {
                    if (b->free && b->size >= size) {
                        // Split block if possible
                        if (b->size > size + sizeof(t_block) + 16) {
                            t_block *new_b = (t_block *)((char *)b + sizeof(t_block) + size);
                            new_b->size = b->size - size - sizeof(t_block);
                            new_b->free = 1;
                            new_b->next = b->next;
                            new_b->prev = b;
                            if (b->next) b->next->prev = new_b;
                            b->next = new_b;
                            b->size = size;
                        }
                        b->free = 0;
                        pthread_mutex_unlock(&g_lock);
                        return (void *)((char *)b + sizeof(t_block));
                    }
                    b = b->next;
                }
            }
            curr = &((*curr)->next);
        }
    }

    // No space found, create new zone
    t_zone *new_z = create_zone(size, type);
    if (!new_z) {
        pthread_mutex_unlock(&g_lock);
        return NULL;
    }
    new_z->next = g_zones;
    g_zones = new_z;
    new_z->blocks->free = 0;
    
    // Large zones take exactly the size requested + headers
    if (type == 2) new_z->blocks->size = size;

    pthread_mutex_unlock(&g_lock);
    return (void *)((char *)new_z->blocks + sizeof(t_block));
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

                // FORWARD COALESCE: Merge with next if free
                if (b->next && b->next->free) {
                    b->size += sizeof(t_block) + b->next->size;
                    b->next = b->next->next;
                    if (b->next) b->next->prev = b;
                }
                // BACKWARD COALESCE: Merge with previous if free
                if (b->prev && b->prev->free) {
                    b->prev->size += sizeof(t_block) + b->size;
                    b->prev->next = b->next;
                    if (b->next) b->next->prev = b->prev;
                }

                // If it's a LARGE zone, we unmap it entirely
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