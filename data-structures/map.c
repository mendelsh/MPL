#include "map.h"

#include <stdlib.h>
#include <string.h>


/**
 * @file map.c
 * @brief Hash map implementation with dynamic resizing and chaining collision resolution
 * 
 * This module implements a hash table (map) data structure that maps string keys to integer values.
 * It uses separate chaining to handle hash collisions and automatically resizes when the load factor
 * exceeds a threshold.
 * 
 * Key Features:
 * - Dynamic resizing using a pre computed prime number table for optimal distribution
 * - Separate chaining for collision resolution
 * - O(1) average case time complexity for add, get, and remove operations
 * - Automatic memory management with proper cleanup
 * 
 * The hash table grows using prime numbers from a static table until exhausted,
 * then switches to doubling the size with odd numbers.
 */

 // TODO: split map into file/'s when it grows too big


#define LOAD_FACTOR_THRESHOLD 0.7

// this table was generated using data-structures/prime_generator.py
static const size_t prime_table[] = {
    1009,
    2027,
    4057,
    8117,
    16249,
    32503,
    65011,
    130027,
    260081,
    520193,
    1040387,
    2080777,
    4161557,
    8323151,
    16646317,
    33292687,
    66585377,
    133170769,
    266341583,
    532683227,
    1065366479,
    2130732959,
    4261465919,
    8522931877,
    17045863781,
    34091727623,
    68183455253,
    136366910509,
    272733821029,
    545467642103
};
static const size_t prime_table_size = 30;

static inline size_t get_next_bucket_count(map_t *map) {
    return map->prime_index < prime_table_size ? prime_table[map->prime_index++] : map->bucket_count * 2 + 1;
}


static bool map_resize(map_t *map) {
    size_t new_bucket_count = get_next_bucket_count(map);
    map_bucket_t **new_buckets = calloc(new_bucket_count, sizeof(map_bucket_t*));
    if (!new_buckets) return false;

    for (size_t i = 0; i < map->bucket_count; i++) {
        map_bucket_t *bucket = map->buckets[i];
        while (bucket) {
            map_bucket_t *next = bucket->next;
            size_t hash = str_hash(bucket->key);
            size_t idx = hash % new_bucket_count;

            bucket->next = new_buckets[idx];
            new_buckets[idx] = bucket;

            bucket = next;
        }
    }

    free(map->buckets);
    map->buckets = new_buckets;
    map->bucket_count = new_bucket_count;

    return true;
}

map_t* map_init() {
    map_t *map = malloc(sizeof(map_t));
    if (!map) return NULL;

    map->size = 0;
    map->prime_index = 0;
    map->bucket_count = get_next_bucket_count(map);
    map->buckets = calloc(map->bucket_count, sizeof(map_bucket_t*));
    if (!map->buckets) {
        free(map);
        return NULL;
    }

    return map;
}

void map_free(map_t *map) {
    if (!map) return;

    for (size_t i = 0; i < map->bucket_count; i++) {
        map_bucket_t *bucket = map->buckets[i];
        while (bucket) {
            map_bucket_t *next = bucket->next;
            free(bucket->key);
            free(bucket);
            bucket = next;
        }
    }

    free(map->buckets);
    free(map);
}

bool map_add(map_t *map, const char *key, int index) {
    if (!map || !key) return false;

    if ((double)(map->size + 1) / map->bucket_count > LOAD_FACTOR_THRESHOLD) {
        if (!map_resize(map)) return false;
    }

    size_t hash = str_hash(key);
    size_t bucket_index = hash % map->bucket_count;

    map_bucket_t *bucket = map->buckets[bucket_index];
    while (bucket) {
        if (strcmp(bucket->key, key) == 0) {
            bucket->index = index;
            return true;
        }
        bucket = bucket->next;
    }

    map_bucket_t *new_bucket = malloc(sizeof(map_bucket_t));
    if (!new_bucket) return false;

    new_bucket->key = strdup(key);
    new_bucket->index = index;
    new_bucket->next = map->buckets[bucket_index];
    map->buckets[bucket_index] = new_bucket;
    map->size++;

    return true;
}

int map_get(map_t *map, const char *key) {
    if (!map || !key) return -1;

    size_t hash = str_hash(key);
    size_t bucket_index = hash % map->bucket_count;

    map_bucket_t *bucket = map->buckets[bucket_index];
    while (bucket) {
        if (strcmp(bucket->key, key) == 0) {
            return bucket->index;
        }
        bucket = bucket->next;
    }

    return -1;
}

void map_remove(map_t *map, const char *key) {
    if (!map || !key) return;

    size_t hash = str_hash(key);
    size_t bucket_index = hash % map->bucket_count;

    map_bucket_t *bucket = map->buckets[bucket_index];
    map_bucket_t *prev = NULL;

    while (bucket) {
        if (strcmp(bucket->key, key) == 0) {
            if (prev) prev->next = bucket->next;
            else map->buckets[bucket_index] = bucket->next;
            
            free(bucket->key);
            free(bucket);
            map->size--;
            return;
        }
        prev = bucket;
        bucket = bucket->next;
    }
}

void map_iterate(map_t *map, void (*func)(const char *key, int index)) {
    if (!map || !func) return;

    for (size_t i = 0; i < map->bucket_count; i++) {
        map_bucket_t *bucket = map->buckets[i];
        while (bucket) {
            func(bucket->key, bucket->index);
            bucket = bucket->next;
        }
    }
}

