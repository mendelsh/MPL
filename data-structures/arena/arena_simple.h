#ifndef ARENA_SIMPLE_H
#define ARENA_SIMPLE_H

#include <stdlib.h>
#include <stdint.h>
#include <stdalign.h>
#include <stddef.h>

typedef uint8_t byte_t;

typedef struct arena_simple_s {
    byte_t *memory;
    size_t size;
    size_t capacity;
} arena_simple_t;

static inline arena_simple_t* as_init(size_t initial_capacity) {
    arena_simple_t *arena = (arena_simple_t*)malloc(sizeof(arena_simple_t));
    if (!arena) return NULL;

    arena->memory = (byte_t*)malloc(initial_capacity);
    if (!arena->memory) {
        free(arena);
        return NULL;
    }

    arena->size = 0;
    arena->capacity = initial_capacity;

    return arena;
}

static inline void as_clear(arena_simple_t *arena) {
    arena->size = 0;
}

static inline void* as_get_memory(arena_simple_t *arena, size_t size) {
    size_t alignment = alignof(max_align_t);
    size_t offset = (arena->size + alignment - 1) & ~(alignment - 1);

    if (offset + size > arena->capacity) {
        size_t new_capacity = (arena->capacity + size) * 2;
        byte_t *new_memory = (byte_t*)realloc(arena->memory, new_capacity);
        if (!new_memory) return NULL;
        arena->memory = new_memory;
        arena->capacity = new_capacity;
    }

    void *ptr = arena->memory + offset;
    arena->size = offset + size;
    return ptr;
}

static inline void as_destroy(arena_simple_t *arena) {
    if (!arena) return;
    if (arena->memory) free(arena->memory);
    free(arena);
}



#endif // ARENA_SIMPLE_H