/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   realloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 16:25:16 by spapyan           #+#    #+#             */
/*   Updated: 2026/02/28 16:25:16 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

void *realloc(void *ptr, size_t size) {
    if (!ptr) return malloc(size);
    if (size == 0) {
        free(ptr);
        return NULL;
    }

    size_t aligned_new = align_size(size);
    pthread_mutex_lock(&g_lock);

    if (!is_ptr_allocated(ptr)) {
        pthread_mutex_unlock(&g_lock);
        return NULL;
    }

    t_block *b = (t_block *)((char *)ptr - sizeof(t_block));
    size_t old_data_size = b->size;
    
    int old_type = (old_data_size <= TINY_MAX) ? 0 : (old_data_size <= SMALL_MAX ? 1 : 2);
    int new_type = (aligned_new <= TINY_MAX) ? 0 : (aligned_new <= SMALL_MAX ? 1 : 2);

    if (old_type == new_type && old_type != 2) {
        if (old_data_size >= aligned_new) {
            split_block(b, aligned_new);
            pthread_mutex_unlock(&g_lock);
            return ptr;
        }
        
        if (b->next && b->next->free &&
            (b->size + sizeof(t_block) + b->next->size) >= aligned_new) {
            b->size += sizeof(t_block) + b->next->size;
            b->next = b->next->next;
            if (b->next) b->next->prev = b;
            split_block(b, aligned_new);
            pthread_mutex_unlock(&g_lock);
            return ptr;
        }
    }

    size_t copy_size = (old_data_size < size) ? old_data_size : size;
    pthread_mutex_unlock(&g_lock);

    void *new_ptr = malloc(size);
    if (!new_ptr) return NULL;

    ft_memcpy(new_ptr, ptr, copy_size);
    free(ptr);
    return new_ptr;
}