#ifndef MALLOC_H
# define MALLOC_H

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

# define ALIGNMENT 16

# define PAGE_SIZE 4096
# define MIN_ZONE_SIZE (PAGE_SIZE * 4)
# define MIN_SPLIT_SIZE 32
# define MIN_REALLOC_DIFF 16

# define TINY_LIMIT 512
# define SMALL_LIMIT 4096

# define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

typedef struct s_block {
    size_t          size;
    int             is_free;
    struct s_block  *next;
    struct s_block  *prev;
} t_block;

typedef struct s_zone {
    size_t          size;
    struct s_zone   *next;
    t_block         *blocks;
    void            *start;
} t_zone;

# define BLOCK_TO_PTR(block) ((void *)((char *)(block) + sizeof(t_block)))

# define PTR_TO_BLOCK(ptr) ((t_block *)((char *)(ptr) - sizeof(t_block)))


extern t_block *g_block_list;
extern t_zone *g_tiny;
extern t_zone *g_small;
extern t_zone *g_large;

void    *malloc(size_t size);
void    *realloc(void *ptr, size_t size);
void    *calloc(size_t nmemb, size_t size);
void    free(void *ptr);
void    show_alloc_mem(void);

t_block *request_memory(size_t size);
void    show_blocks(t_zone *zone);

t_block *find_free_block(t_zone *zone, size_t size);
t_block *find_best_fit_block(size_t size);

void    split_block(t_block *block, size_t size);
void    coalesce_blocks(t_block *block);

void    ft_bzero(void *ptr, size_t size);

t_zone  **get_zone(size_t size);
t_zone  *allocate_new_zone(t_zone *prev, size_t size);

#endif