/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 16:14:54 by spapyan           #+#    #+#             */
/*   Updated: 2026/02/28 16:14:54 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

void *malloc(size_t size) {
    pthread_mutex_lock(&g_lock);
    void *res = internal_malloc(size);
    pthread_mutex_unlock(&g_lock);
    return res;
}