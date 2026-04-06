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