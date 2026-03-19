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

// void *realloc(void *ptr, size_t size) {
//     if (!ptr) return malloc(size);
//     if (size == 0) {
//         free(ptr);
//         return NULL;
//     }

//     t_block *b = (t_block *)((char *)ptr - sizeof(t_block));
//     size_t aligned_new = align_size(size);

//     // Если текущий блок уже подходит по размеру
//     if (b->size >= aligned_new) return ptr;

//     void *new_ptr = malloc(size);
//     if (new_ptr) {
//         // TODO: Create ft_memcpy
//         ft_memcpy(new_ptr, ptr, b->size);
//         free(ptr);
//     }
//     return new_ptr;
// }

void *realloc(void *ptr, size_t size) {
    if (!ptr) return malloc(size);
    if (size == 0) {
        free(ptr);
        return NULL;
    }

    t_block *b = (t_block *)((char *)ptr - sizeof(t_block));
    size_t aligned_new = align_size(size);

    // 1. Проверяем, не изменился ли тип зоны (TINY/SMALL/LARGE)
    int old_type = (b->size <= TINY_MAX) ? 0 : (b->size <= SMALL_MAX ? 1 : 2);
    int new_type = (aligned_new <= TINY_MAX) ? 0 : (aligned_new <= SMALL_MAX ? 1 : 2);

    // 2. Если тип тот же и места хватает — просто возвращаем
    if (old_type == new_type && b->size >= aligned_new) {
        // Тут можно добавить split_block(b, aligned_new), если места осталось слишком много
        // CHECKOUT
        split_block(b, size);
        return ptr;
    }

    // 3. ОПЦИОНАЛЬНО: Попробовать объединиться со следующим блоком (b->next)
    // если (old_type == new_type && b->next && b->next->free && ...)

    // 4. Если ничего не помогло — классический перенос
    void *new_ptr = malloc(size);
    if (new_ptr) {
        // КРИТИЧНО: копируем только то, что реально было у пользователя!
        // Если у тебя нет поля b->user_size, используй b->size, но это менее точно.
        size_t copy_size = (b->size < aligned_new) ? b->size : aligned_new;
        ft_memcpy(new_ptr, ptr, copy_size);
        free(ptr);
    }
    return new_ptr;
}