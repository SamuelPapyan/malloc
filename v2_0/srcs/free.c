/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 16:18:44 by spapyan           #+#    #+#             */
/*   Updated: 2026/02/28 16:18:44 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

void free(void *ptr) {
    if (!ptr) return;
    pthread_mutex_lock(&g_lock);
    internal_free(ptr);
    pthread_mutex_unlock(&g_lock);
}