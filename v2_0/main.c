/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 16:24:11 by spapyan           #+#    #+#             */
/*   Updated: 2026/03/02 16:24:11 by spapyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "malloc.h"

// void show_alloc_mem(); // Prototype from your library

#define THREAD_COUNT 4
#define ALLOC_COUNT 50

void *thread_routine(void *arg) {
    int id = *(int *)arg;
    void *ptrs[ALLOC_COUNT];

    for (int i = 0; i < ALLOC_COUNT; i++) {
        // Mix of TINY, SMALL, and LARGE
        size_t size = (i % 3 == 0) ? 64 : (i % 3 == 1 ? 512 : 4096 * 2);
        ptrs[i] = malloc(size);
        if (ptrs[i]) {
            memset(ptrs[i], 0x41 + id, size - 1); // Fill with A, B, C...
            ((char *)ptrs[i])[size - 1] = '\0';
        }
    }

    for (int i = 0; i < ALLOC_COUNT; i++) {
        if (i % 2 == 0) free(ptrs[i]); // Partial free to test fragmentation
    }
    return NULL;
}

int main() {
    pthread_t threads[THREAD_COUNT];
    int ids[THREAD_COUNT];

    ft_putendl("Starting multi-threaded stress test...");

    for (int i = 0; i < THREAD_COUNT; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, thread_routine, &ids[i]);
    }

    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }
    ft_putendl("--- Memory State after test ---");
    show_alloc_mem();
    
    return 0;
}