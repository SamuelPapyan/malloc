/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   calloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/06 11:46:52 by spapyan           #+#    #+#             */
/*   Updated: 2026/04/06 11:46:52 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

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