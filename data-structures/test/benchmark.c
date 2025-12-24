#include "uthash.h"
#include "../map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

/**
 * @file uthash_benchmark.c
 * @brief Compare your map implementation with uthash
 */

// ============================================================================
// UTHASH ENTRY
// ============================================================================

typedef struct {
    char key[32];
    int value;
    UT_hash_handle hh;
} uthash_entry_t;

// ============================================================================
// TIME
// ============================================================================

double get_time(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

// ============================================================================
// UTHASH SORT COMPARATOR  (FIX #2)
// ============================================================================

int uthash_key_cmp(uthash_entry_t *a, uthash_entry_t *b) {
    return strcmp(a->key, b->key);
}

// ============================================================================
// MAP ITER CALLBACK (FIX #3)
// ============================================================================

static int iter_count;

void count_entries(const char *k, int v) {
    (void)k;
    (void)v;
    iter_count++;
}

// ============================================================================
// UTHASH BENCHMARK
// ============================================================================

void benchmark_uthash(int n) {
    printf("\n=== uthash Benchmark: %d entries ===\n", n);

    uthash_entry_t *hash = NULL;
    double start, insert_time, lookup_time, delete_time;

    // INSERT
    start = get_time();
    for (int i = 0; i < n; i++) {
        uthash_entry_t *entry = malloc(sizeof(*entry));
        snprintf(entry->key, sizeof(entry->key), "key%d", i);
        entry->value = i * 10;
        HASH_ADD_STR(hash, key, entry);
    }
    insert_time = get_time() - start;

    printf("[uthash] Inserted %u entries in %.3f seconds\n",
           HASH_COUNT(hash), insert_time);

    // LOOKUP
    start = get_time();
    int hits = 0;
    int lookups = n < 1000000 ? 100000 : 1000000;

    for (int i = 0; i < lookups; i++) {
        char key[32];
        snprintf(key, sizeof(key), "key%d", rand() % n);

        uthash_entry_t *found;
        HASH_FIND_STR(hash, key, found);
        if (found) hits++;
    }
    lookup_time = get_time() - start;

    printf("[uthash] Lookup %d keys in %.3f seconds (%d hits)\n",
           lookups, lookup_time, hits);

    // ITERATE
    start = get_time();
    uthash_entry_t *e, *tmp;
    int count = 0;
    HASH_ITER(hh, hash, e, tmp) {
        count++;
    }
    printf("[uthash] Iterated %d entries in %.3f seconds\n",
           count, get_time() - start);

    // DELETE 10%
    start = get_time();
    int deletes = n / 10;
    for (int i = 0; i < deletes; i++) {
        char key[32];
        snprintf(key, sizeof(key), "key%d", i);
        uthash_entry_t *found;
        HASH_FIND_STR(hash, key, found);
        if (found) {
            HASH_DEL(hash, found);
            free(found);
        }
    }
    delete_time = get_time() - start;

    printf("[uthash] Deleted %d entries in %.3f seconds\n",
           deletes, delete_time);

    // CLEANUP
    start = get_time();
    HASH_ITER(hh, hash, e, tmp) {
        HASH_DEL(hash, e);
        free(e);
    }
    printf("[uthash] Cleanup in %.3f seconds\n", get_time() - start);
}

// ============================================================================
// YOUR MAP BENCHMARK
// ============================================================================

void benchmark_your_map(int n) {
    printf("\n=== Your Map Benchmark: %d entries ===\n", n);

    map_t *map = map_init();
    double start;

    // INSERT
    start = get_time();
    for (int i = 0; i < n; i++) {
        char key[32];
        snprintf(key, sizeof(key), "key%d", i);
        map_add(map, key, i * 10);
    }
    printf("[your_map] Inserted %zu entries in %.3f seconds\n",
           map->size, get_time() - start);

    // LOOKUP
    start = get_time();
    int hits = 0;
    int lookups = n < 1000000 ? 100000 : 1000000;

    for (int i = 0; i < lookups; i++) {
        char key[32];
        snprintf(key, sizeof(key), "key%d", rand() % n);
        if (map_get(map, key) >= 0) hits++;
    }
    printf("[your_map] Lookup %d keys in %.3f seconds (%d hits)\n",
           lookups, get_time() - start, hits);

    // ITERATE
    iter_count = 0;
    start = get_time();
    map_iterate(map, count_entries);
    printf("[your_map] Iterated %d entries in %.3f seconds\n",
           iter_count, get_time() - start);

    // DELETE 10%
    start = get_time();
    for (int i = 0; i < n / 10; i++) {
        char key[32];
        snprintf(key, sizeof(key), "key%d", i);
        map_remove(map, key);
    }
    printf("[your_map] Deleted %d entries in %.3f seconds\n",
           n / 10, get_time() - start);

    // CLEANUP
    start = get_time();
    map_free(map);
    printf("[your_map] Cleanup in %.3f seconds\n", get_time() - start);
}

// ============================================================================
// DEMO
// ============================================================================

void demo_uthash_features(void) {
    printf("\n=== uthash Features Demo ===\n");

    uthash_entry_t *users = NULL;
    const char *names[] = {"alice", "bob", "charlie", "david", "eve"};

    for (int i = 0; i < 5; i++) {
        uthash_entry_t *u = malloc(sizeof(*u));
        snprintf(u->key, sizeof(u->key), "%s", names[i]);
        u->value = (i + 1) * 1000;
        HASH_ADD_STR(users, key, u);
    }

    HASH_SORT(users, uthash_key_cmp);

    uthash_entry_t *u, *tmp;
    HASH_ITER(hh, users, u, tmp) {
        printf("  %s -> %d\n", u->key, u->value);
    }

    HASH_ITER(hh, users, u, tmp) {
        HASH_DEL(users, u);
        free(u);
    }
}

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char **argv) {
    srand(time(NULL));

    if (argc < 3) {
        printf("Usage:\n");
        printf("  %s map N      # run your map only\n", argv[0]);
        printf("  %s uthash N   # run uthash only\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[2]);

    if (strcmp(argv[1], "map") == 0) benchmark_your_map(n);    
    else if (strcmp(argv[1], "uthash") == 0) benchmark_uthash(n);    
    else {
        printf("Unknown mode: %s\n", argv[1]);
        return 1;
    }

    return 0;
}

