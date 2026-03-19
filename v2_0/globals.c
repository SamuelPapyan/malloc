#include "malloc.h"

pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
t_zone *g_zones = NULL;