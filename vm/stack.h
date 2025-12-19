#ifndef STACK_H
#define STACK_H

#include "builtin.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SLICE_TYPE(name, type)      \
typedef struct {                    \
    bool is_on_stack;               \
    type *data;                     \
    size_t size;                    \
    size_t capacity;                \
} name;

#define FUNCS_DEF_INIT_PUSH_POP_FREE(name, type_name, type)                   \
static inline void name##_init(type_name *name, size_t capacity, type *buff); \
static inline void name##_push(type_name *name, type item);                   \
static inline type name##_pop(type_name *name);                               \
static inline void name##_free(type_name *name);

#define STACK_GROWTH_FACTOR 2
#define STACK_SHRINK_FACTOR 2
#define STACK_SHRINK_THRESHOLD 8
#define STACK_MIN_CAPACITY 8


#define SLICE_INIT(slice, capacity, buff, type) \
    if (capacity == 0) capacity = STACK_MIN_CAPACITY;                   \
    if (buff) {                                                         \
        slice->is_on_stack = true;                                      \
        slice->data = buff;                                             \
    } else {                                                            \
        slice->is_on_stack = false;                                     \
        slice->data = malloc(capacity * sizeof(type));                  \
        if (!slice->data) {                                             \
            fprintf(stderr, "Failed to allocate memory for stack\n");   \
            exit(EXIT_FAILURE);                                         \
        }                                                               \
    }                                                                   \
    slice->size = 0;                                                    \
    slice->capacity = capacity

#define SLICE_PUSH(slice, item, type)                                                       \
    if (slice->size >= slice->capacity) {                                                   \
        type *new_data = malloc(slice->capacity * STACK_GROWTH_FACTOR * sizeof(type));      \
        if (!new_data) {                                                                    \
            fprintf(stderr, "Failed to allocate memory for stack\n");                       \
            exit(EXIT_FAILURE);                                                             \
        }                                                                                   \
        memcpy(new_data, slice->data, slice->capacity * sizeof(type));                      \
        if (!slice->is_on_stack) free(slice->data);                                         \
        slice->data = new_data;                                                             \
        slice->capacity *= STACK_GROWTH_FACTOR;                                             \
        slice->is_on_stack = false;                                                         \
    }                                                                                       \
    slice->data[slice->size++] = item

#define SLICE_POP(slice, type)                                                                                  \
    if (slice->size == 0) {                                                                                     \
        fprintf(stderr, "Stack is empty\n");                                                                    \
        exit(EXIT_FAILURE);                                                                                     \
    }                                                                                                           \
    slice->size--;                                                                                              \
    if (!slice->is_on_stack && slice->size > 0 && slice->capacity >= slice->size * STACK_SHRINK_THRESHOLD) {    \
        size_t new_cap = slice->size * STACK_SHRINK_FACTOR;                                                     \
        if (new_cap < STACK_MIN_CAPACITY) return slice->data[slice->size];                                      \
        type *new_data = malloc(new_cap * sizeof(type));                                                        \
        if (!new_data) {                                                                                        \
            fprintf(stderr, "Failed to allocate memory for stack\n");                                           \
            exit(EXIT_FAILURE);                                                                                 \
        }                                                                                                       \
        memcpy(new_data, slice->data, slice->size * sizeof(type));                                              \
        free(slice->data);                                                                                      \
        slice->data = new_data;                                                                                 \
        slice->capacity = new_cap;                                                                              \
    }                                                                                                           \
    return slice->data[slice->size]

#define SLICE_FREE(slice)                                                                                       \
    if (!slice->is_on_stack && slice->data) free(slice->data);                                                  \
    slice->data = NULL;                                                                                         \
    slice->size = 0;                                                                                            \
    slice->capacity = 0


#define FUNCS_IMPL_INIT_PUSH_POP_FREE(name, type_name, type)                        \
static inline void name##_init(type_name *name, size_t capacity, type *buff) {      \
    SLICE_INIT(name, capacity, buff, type);                                         \
}                                                                                   \
static inline void name##_push(type_name *name, type item) {                        \
    SLICE_PUSH(name, item, type);                                                   \
}                                                                                   \
static inline type name##_pop(type_name *name) {                                    \
    SLICE_POP(name, type);                                                          \
}                                                                                   \
static inline void name##_free(type_name *name) {                                   \
    SLICE_FREE(name);                                                               \
}



#endif // STACK_H
