#ifndef TABLE_H
#define TABLE_H

#include <stdlib.h>
#include <stdbool.h>

#ifndef TABLE_LOAD_FACTOR
#define TABLE_LOAD_FACTOR 0.7
#endif


///////////////// User Macros ///////////////////

#define TABLE_EQ(type, a, b) TABLE_EQ_##type((a), (b))
#define TABLE_FREE_KEY(type, key) TABLE_FREE_KEY_##type(key)
#define TABLE_FREE_VALUE(type, value) TABLE_FREE_VALUE_##type(value)
#define NO_FREE void


///////////////// Hash Functions ///////////////////

static inline size_t str_hash(const char *str) {
    size_t hash = 5381;
    int c;
    while ((c = *str++)) hash = ((hash << 5) + hash) + (unsigned char)c;
    return hash;
}


///////////////// Error Codes ///////////////////

enum {
    TABLE_SUCCESS = 1,
    TABLE_ERR_ALLOC = -1,
    TABLE_ERR_ALREADY_EXISTS = -2,
    TABLE_ERR_NOT_PROVIDED = -3,
    TABLE_ERR_NOT_FOUND = -4
};

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

#define DEF_TABLE_GET_NEXT_BUCKET_COUNT(table)                                                                      \
static inline size_t get_next_bucket_count(table##_t *table) {                                                      \
    return table->prime_index < prime_table_size ? prime_table[table->prime_index++] : table->bucket_count * 2 + 1; \
}


////////////////// Function Macro ////////////////////

#define DEF_TABLE_RESIZE(table_name)                                                                \
bool table_name##_resize(table_name##_t *table) {                                                   \
    size_t new_bucket_count = get_next_bucket_count(table);                                         \
    table_name##_bucket_t **new_buckets = calloc(new_bucket_count, sizeof(table_name##_bucket_t*)); \
    if (!new_buckets) return false;                                                                 \
    for (size_t i = 0; i < table->bucket_count; i++) {                                              \
        table_name##_bucket_t *bucket = table->buckets[i];                                          \
        while (bucket) {                                                                            \
            table_name##_bucket_t *next = bucket->next;                                             \
            size_t hash = table_name##_hash(bucket->key);                                           \
            size_t idx = hash % new_bucket_count;                                                   \
            bucket->next = new_buckets[idx];                                                        \
            new_buckets[idx] = bucket;                                                              \
            bucket = next;                                                                          \
        }                                                                                           \
    }                                                                                               \
    free(table->buckets);                                                                           \
    table->buckets = new_buckets;                                                                   \
    table->bucket_count = new_bucket_count;                                                         \
    return true;                                                                                    \
}



#define DEF_TABLE_INIT(table_name)                                                  \
table_name##_t* table_name##_init() {                                               \
    table_name##_t *table = malloc(sizeof(table_name##_t));                         \
    if (!table) return NULL;                                                        \
    table->size = 0;                                                                \
    table->prime_index = 0;                                                         \
    table->bucket_count = get_next_bucket_count(table);                             \
    table->buckets = calloc(table->bucket_count, sizeof(table_name##_bucket_t*));   \
    if (!table->buckets) {                                                          \
        free(table);                                                                \
        return NULL;                                                                \
    }                                                                               \
    return table;                                                                   \
}

#define DEF_TABLE_FREE(table_name, key_type, value_type)    \
void table_name##_free(table_name##_t *table) {             \
    if (!table) return;                                     \
    for (size_t i = 0; i < table->bucket_count; i++) {      \
        table_name##_bucket_t *bucket = table->buckets[i];  \
        while (bucket) {                                    \
            table_name##_bucket_t *next = bucket->next;     \
            TABLE_FREE_KEY(key_type, bucket->key);          \
            TABLE_FREE_VALUE(value_type, bucket->value);    \
            free(bucket);                                   \
            bucket = next;                                  \
        }                                                   \
    }                                                       \
    free(table->buckets);                                   \
    free(table);                                            \
}

#define DEF_TABLE_ADD(table_name, key_type, value_type)                             \
int table_name##_add(table_name##_t *table, key_type key, value_type value) {       \
    if (!table) return TABLE_ERR_NOT_PROVIDED;                                      \
    if ((double)(table->size + 1) / table->bucket_count > TABLE_LOAD_FACTOR) {      \
        if (!table_name##_resize(table)) return TABLE_ERR_ALLOC;                    \
    }                                                                               \
    size_t hash = table_name##_hash(key);                                           \
    size_t bucket_index = hash % table->bucket_count;                               \
    table_name##_bucket_t *bucket = table->buckets[bucket_index];                   \
    while (bucket) {                                                                \
        if (TABLE_EQ(key_type, bucket->key, key)) return TABLE_ERR_ALREADY_EXISTS;  \
        bucket = bucket->next;                                                      \
    }                                                                               \
    table_name##_bucket_t *new_bucket = malloc(sizeof(table_name##_bucket_t));      \
    if (!new_bucket) return TABLE_ERR_ALLOC;                                        \
    new_bucket->key = key;                                                          \
    new_bucket->value = value;                                                      \
    new_bucket->next = table->buckets[bucket_index];                                \
    table->buckets[bucket_index] = new_bucket;                                      \
    table->size++;                                                                  \
    return TABLE_SUCCESS;                                                           \
}

#define DEF_TABLE_GET(table_name, key_type, value_type)                                                                         \
table_name##_result_t table_name##_get(table_name##_t *table, key_type key) {                                                   \
    if (!table) return (table_name##_result_t){.err = TABLE_ERR_NOT_PROVIDED};                                                  \
    size_t hash = table_name##_hash(key);                                                                                       \
    size_t bucket_index = hash % table->bucket_count;                                                                           \
    table_name##_bucket_t *bucket = table->buckets[bucket_index];                                                               \
    while (bucket) {                                                                                                            \
        if (TABLE_EQ(key_type, bucket->key, key)) return (table_name##_result_t){.err = TABLE_SUCCESS, .value = bucket->value}; \
        bucket = bucket->next;                                                                                                  \
    }                                                                                                                           \
    return (table_name##_result_t){.err = TABLE_ERR_NOT_FOUND};                                                                 \
}

#define DEF_TABLE_SET(table_name, key_type, value_type)                         \
int table_name##_set(table_name##_t *table, key_type key, value_type value) {   \
    if (!table) return TABLE_ERR_NOT_PROVIDED;                                  \
    size_t hash = table_name##_hash(key);                                       \
    size_t bucket_index = hash % table->bucket_count;                           \
    table_name##_bucket_t *bucket = table->buckets[bucket_index];               \
    while (bucket) {                                                            \
        if (TABLE_EQ(key_type, bucket->key, key)) {                             \
            TABLE_FREE_VALUE(value_type, bucket->value);                        \
            bucket->value = value;                                              \
            return TABLE_SUCCESS;                                               \
        }                                                                       \
        bucket = bucket->next;                                                  \
    }                                                                           \
    return TABLE_ERR_NOT_FOUND;                                                 \
}

#define DEF_TABLE_REMOVE(table_name, key_type, value_type)              \
int table_name##_remove(table_name##_t *table, key_type key) {          \
    if (!table) return TABLE_ERR_NOT_PROVIDED;                          \
    size_t hash = table_name##_hash(key);                               \
    size_t bucket_index = hash % table->bucket_count;                   \
    table_name##_bucket_t *bucket = table->buckets[bucket_index];       \
    table_name##_bucket_t *prev = NULL;                                 \
    while (bucket) {                                                    \
        if (TABLE_EQ(key_type, bucket->key, key)) {                     \
            if (prev) prev->next = bucket->next;                        \
            else table->buckets[bucket_index] = bucket->next;           \
            TABLE_FREE_KEY(key_type, bucket->key);                      \
            TABLE_FREE_VALUE(value_type, bucket->value);                \
            free(bucket);                                               \
            table->size--;                                              \
            return TABLE_SUCCESS;                                       \
        }                                                               \
        prev = bucket;                                                  \
        bucket = bucket->next;                                          \
    }                                                                   \
    return TABLE_ERR_NOT_FOUND;                                         \
}



///////////////// Definition Macro ///////////////////

#define TABLE_T(table_name, key_type, value_type)   \
typedef struct {                                    \
    int err;                                        \
    value_type value;                               \
} table_name##_result_t;                            \
typedef struct table_name##_bucket_s {              \
    key_type key;                                   \
    value_type value;                               \
    struct table_name##_bucket_s *next;             \
} table_name##_bucket_t;                            \
typedef struct table_name##_s {                     \
    table_name##_bucket_t **buckets;                \
    size_t bucket_count;                            \
    size_t size;                                    \
    size_t prime_index;                             \
} table_name##_t;                                   \
DEF_TABLE_GET_NEXT_BUCKET_COUNT(table_name)             \
DEF_TABLE_RESIZE(table_name)                            \
DEF_TABLE_INIT(table_name)                              \
DEF_TABLE_FREE(table_name, key_type, value_type)        \
DEF_TABLE_GET(table_name, key_type, value_type)         \
DEF_TABLE_SET(table_name, key_type, value_type)         \
DEF_TABLE_ADD(table_name, key_type, value_type)         \
DEF_TABLE_REMOVE(table_name, key_type, value_type)




#endif // TABLE_H
