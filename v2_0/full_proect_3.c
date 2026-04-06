#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>
#include <errno.h>

#define TINY_MAX 128
#define SMALL_MAX 1024

#define TINY_HEAP_SIZE ((TINY_MAX + sizeof(t_block)) * 128)
#define SMALL_HEAP_SIZE ((SMALL_MAX + sizeof(t_block)) * 128)

typedef struct s_block {
    size_t          size;
    struct s_block  *next;
    struct s_block  *prev;
    int             free;
    int             padding;
} t_block __attribute__((aligned(16)));

typedef struct s_zone {
    size_t          type; // 0: TINY, 1: SMALL, 2: LARGE
    size_t          total_size;
    struct s_zone   *next;
    t_block         *blocks;
} t_zone __attribute__((aligned(16)));

pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
t_zone *g_zones = NULL;

void    ft_putchar(char c);
void    ft_putstr(char *str);
void    ft_putendl(char *str);
void    ft_putnbr(int nbr);
void	ft_puthex(int nbr);
void	ft_putuendl(unsigned char *str);
void	ft_putptr(void *ptr);
void	ft_putform(int nbr);

size_t  align_size(size_t size);
size_t  get_zone_size(size_t size, int type);
int     is_zone_empty(t_zone *zone);
int     should_unmap_zone(t_zone *z);

void split_block(t_block *b, size_t size);
t_zone *create_zone(size_t size, int type);

void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
void *calloc(size_t nmemb, size_t size);
int is_ptr_allocated(void *ptr);

void show_alloc_mem();

void free(void *ptr) {
    if (!ptr) return;
    pthread_mutex_lock(&g_lock);

    t_zone **prev_z = &g_zones;
    t_zone *z = g_zones;

    while (z) {
        t_block *b = z->blocks;
        while (b) {
            if ((void *)((char *)b + sizeof(t_block)) == ptr) {
                if (b->free) {
                    pthread_mutex_unlock(&g_lock);
                    return;
                }
                b->free = 1;
                // Coalescing
                if (b->next && b->next->free) {
                    b->size += sizeof(t_block) + b->next->size;
                    b->next = b->next->next;
                    if (b->next) b->next->prev = b;
                }
                if (b->prev && b->prev->free) {
                    t_block *prev_b = b->prev;
                    prev_b->size += sizeof(t_block) + b->size;
                    prev_b->next = b->next;
                    if (b->next) b->next->prev = prev_b;
                    b = prev_b;
                }
                if (b->prev == NULL)
                    z->blocks = b;
                if (is_zone_empty(z) && should_unmap_zone(z)) {
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

void *malloc(size_t size) {
    if (size < 0) return NULL;
    if (size == 0) size = 1;
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

void *realloc(void *ptr, size_t size) {
    if (!ptr) return malloc(size);
    if (size == 0) {
        free(ptr);
        return NULL;
    }
    pthread_mutex_lock(&g_lock);
    if (!is_ptr_allocated(ptr)) {
        pthread_mutex_unlock(&g_lock);
        return NULL;
    }

    t_block *b = (t_block *)((char *)ptr - sizeof(t_block));
    size_t aligned_new = align_size(size);
    
    int old_type = (b->size <= TINY_MAX) ? 0 : (b->size <= SMALL_MAX ? 1 : 2);
    int new_type = (aligned_new <= TINY_MAX) ? 0 : (aligned_new <= SMALL_MAX ? 1 : 2);

    if (old_type == new_type && b->size >= aligned_new) {
        split_block(b, aligned_new);
        pthread_mutex_unlock(&g_lock);
        return ptr;
    }
    size_t old_size = b->size;
    pthread_mutex_unlock(&g_lock);

    void *new_ptr = malloc(size);
    if (new_ptr) {
        ft_memcpy(new_ptr, ptr, (old_size < size) ? old_size : size);
        free(ptr);
    }
    return new_ptr;
}

int is_zone_empty(t_zone *zone) {
    t_block *b = zone->blocks;

    if (b && b->free && b->next == NULL && b->prev == NULL) {
        return 1;
    }
    return 0;
}

int should_unmap_zone(t_zone *z) {
    if (z->type == 2) {
        return 1;
    }

    t_zone *curr = g_zones;
    int empty_found = 0;

    while (curr) {
        if (curr->type == z->type && is_zone_empty(curr)) {
            empty_found++;
        }
        if (empty_found > 1) {
            return 1;
        }
        curr = curr->next;
    }
    return 0;
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
        t_zone *z = g_zones;
        int title_printed = 0;

        while (z) {
            if (z->type == (size_t)i) {
                if (!title_printed) {
                    ft_putstr(titles[i]);
                    ft_putstr(" : ");
                    ft_putptr((void *)z); // Печатаем адрес зоны
                    ft_putendl("");
                    title_printed = 1;
                }
                t_block *b = z->blocks;
                while (b) {
                    if (!b->free) {
                        void *start = (char *)b + sizeof(t_block);
                        void *end = (char *)start + b->size;
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
    ft_putstr("Total : ");
    ft_putnbr(total);
    ft_putendl(" bytes");
    pthread_mutex_unlock(&g_lock);
}

void split_block(t_block *b, size_t size) {
    size = align_size(size);

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

size_t get_zone_size(size_t size, int type) {
    size_t page = sysconf(_SC_PAGESIZE);
    size_t total;

    if (type == 2) total = size + sizeof(t_zone) + sizeof(t_block);
    else if (type == 1) total = SMALL_HEAP_SIZE + sizeof(t_zone);
    else total = TINY_HEAP_SIZE + sizeof(t_zone);

    return (total + page - 1) & ~(page - 1);
}

size_t align_size(size_t size) {
    return (size + 15) & ~15;
}

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;

    if (!dest && !src) return NULL;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

t_zone *create_zone(size_t size, int type) {
    size_t total_needed = get_zone_size(size, type);
    t_zone *zone = mmap(NULL, total_needed, PROT_READ | PROT_WRITE, 
                        MAP_ANON | MAP_PRIVATE, -1, 0);
    if (zone == MAP_FAILED) {
        errno = ENOMEM;
        return NULL;
    }

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

void *calloc(size_t nmemb, size_t size) {
    if (nmemb == 0 || size == 0) return malloc(0);
    if (nmemb > (size_t)-1 / size) {
        errno = ENOMEM;
        return NULL;
    }

    size_t total = nmemb * size;
    void *ptr = malloc(total);
    if (ptr) {
        unsigned char *p = (unsigned char *)ptr;
        for (size_t i = 0; i < total; i++) {
            p[i] = 0;
        }
    }
    return ptr;
}

int is_ptr_allocated(void *ptr) {
    t_zone *z = g_zones;
    while (z) {
        t_block *b = z->blocks;
        while (b) {
            if ((void *)((char *)b + sizeof(t_block)) == ptr) {
                return (b->free == 0); // Возвращаем истину, если блок занят
            }
            b = b->next;
        }
        z = z->next;
    }
    return 0;
}