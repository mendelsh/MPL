#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <stdbool.h>

#ifndef LOAD_FACTOR_THRESHOLD
#define LOAD_FACTOR_THRESHOLD 0.7
#endif

static inline size_t str_hash(const char *str) {
    size_t hash = 5381;
    int c;
    while ((c = *str++)) hash = ((hash << 5) + hash) + (unsigned char)c;
    return hash;
}

typedef struct map_bucket_s {
    char *key;
    int index;
    struct map_bucket_s *next;
} map_bucket_t;

typedef struct map_s {
    map_bucket_t **buckets;
    size_t bucket_count;
    size_t size;
    size_t prime_index;
} map_t;

map_t* map_init();
void map_free(map_t *map);
bool map_add(map_t *map, const char *key, int index);
int map_get(map_t *map, const char *key);
bool map_set(map_t *map, const char *key, int new_index);
void map_remove(map_t *map, const char *key);
void map_iterate(map_t *map, void (*func)(const char *key, int index));

#define MAP_ITERATE(map, i, k, v, code)                             \
    size_t i;                                                       \
    for (i = 0; i < map->bucket_count; i++) {                       \
        map_bucket_t *_bucket_##__COUNTER__ = map->buckets[i];      \
        while (_bucket_##__COUNTER__) {                             \
            const char *k = _bucket_##__COUNTER__->key;             \
            int v = _bucket_##__COUNTER__->index;                   \
            { code }                                                \
            _bucket_##__COUNTER__ = _bucket_##__COUNTER__->next;    \
        }                                                           \
    }


#endif // MAP_H