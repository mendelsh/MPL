#ifndef ARENA_CHAIN_H
#define ARENA_CHAIN_H

#include <stdlib.h>
#include <stdint.h>
#include <stdalign.h>
#include <stddef.h>

typedef uint8_t byte_t;

typedef struct ac_bucket_s {
    byte_t *memory;
    size_t offset;
    size_t capacity;
    struct ac_bucket_s *next;
    struct ac_bucket_s *prev;
} ac_bucket_t;

typedef struct {
    ac_bucket_t *head;
    ac_bucket_t *curr;
    size_t size;
} arena_chain_t;

static inline arena_chain_t* ac_init(size_t initial_capacity) {
    arena_chain_t *arena = (arena_chain_t*)malloc(sizeof(arena_chain_t));
    if (!arena) return NULL;

    arena->head = (ac_bucket_t*)malloc(sizeof(ac_bucket_t));
    if (!arena->head) {
        free(arena);
        return NULL;
    }

    arena->head->memory = (byte_t*)malloc(initial_capacity);
    if (!arena->head->memory) {
        free(arena->head);
        free(arena);
        return NULL;
    }

    arena->head->offset = 0;
    arena->head->capacity = initial_capacity;
    arena->head->next = NULL;
    arena->head->prev = NULL;

    arena->curr = arena->head;
    arena->size = sizeof(arena_chain_t) + sizeof(ac_bucket_t) + initial_capacity;

    return arena;
}

static inline void ac_clear(arena_chain_t *arena) {
    if (!arena) return;
    arena->head->offset = 0;
    arena->curr = arena->head;
}

static inline void* ac_get_memory(arena_chain_t *arena, size_t size) {
    size_t alignment = alignof(max_align_t);
    size_t offset = (arena->curr->offset + alignment - 1) & ~(alignment - 1);

    if (offset + size > arena->curr->capacity) {
        goto new_bucket_label;

        loop_label:
            if (offset + size <= arena->curr->capacity) goto offset_label;
            else {
                free(arena->curr->memory);
                if (arena->curr->prev) {
                    if (arena->curr->next) {
                        arena->curr->prev->next = arena->curr->next;
                        free(arena->curr);
                        arena->curr = arena->curr->prev;
                    } else {
                        free(arena->curr);
                        arena->curr = arena->curr->prev;
                        arena->curr->next = NULL;
                    }
                }

                goto new_bucket_label;
            }

        new_bucket_label:
            if (!arena->curr->next) {
                size_t new_capacity = (size + arena->curr->capacity) * 2;
                ac_bucket_t *new_bucket = (ac_bucket_t*)malloc(sizeof(ac_bucket_t));
                if (!new_bucket) return NULL;

                new_bucket->memory = (byte_t*)malloc(new_capacity);
                if (!new_bucket->memory) {
                    free(new_bucket);
                    return NULL;
                }

                new_bucket->offset = 0;
                new_bucket->capacity = new_capacity;
                new_bucket->next = NULL;
                new_bucket->prev = arena->curr;

                arena->curr->next = new_bucket;
                arena->curr = new_bucket;
                arena->size += sizeof(ac_bucket_t) + new_capacity;
            } else {
                arena->curr = arena->curr->next;
                goto loop_label;
            }

        offset_label:
            offset = 0;
    }

    void *ptr = arena->curr->memory + offset;
    arena->curr->offset = offset + size;
    return ptr;
}

static inline void ac_destroy(arena_chain_t *arena) {
    if (!arena) return;

    ac_bucket_t *bucket = arena->head;
    while (bucket) {
        ac_bucket_t *next = bucket->next;
        if (bucket->memory) free(bucket->memory);
        free(bucket);
        bucket = next;
    }

    free(arena);
}


#endif // ARENA_CHAIN_H