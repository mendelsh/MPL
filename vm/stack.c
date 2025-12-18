
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stack.h"

#define STACK_GROWTH_FACTOR 2
#define STACK_SHRINK_FACTOR 2
#define STACK_SHRINK_THRESHOLD 8
#define STACK_MIN_CAPACITY 8

void stack_init(stack_slice_t *stack, size_t capacity, type_t *buff) {
    if (capacity == 0) capacity = STACK_MIN_CAPACITY;

    if (buff) {
        stack->is_on_stack = true;
        stack->data = buff;
    } else {
        stack->is_on_stack = false;
        stack->data = malloc(capacity * sizeof(type_t));
        if (!stack->data) {
            fprintf(stderr, "Failed to allocate memory for stack\n");
            exit(EXIT_FAILURE);
        }
    }

    stack->size = 0;
    stack->capacity = capacity;
}

void stack_push(stack_slice_t *stack, type_t item) {
    if (stack->size >= stack->capacity) {
        type_t *new_data = malloc(stack->capacity * STACK_GROWTH_FACTOR * sizeof(type_t));
        if (!new_data) {
            fprintf(stderr, "Failed to allocate memory for stack\n");
            exit(EXIT_FAILURE);
        }

        memcpy(new_data, stack->data, stack->capacity * sizeof(type_t));
        if (!stack->is_on_stack) free(stack->data);
        stack->data = new_data;
        stack->capacity *= STACK_GROWTH_FACTOR;
        stack->is_on_stack = false;
    }

    stack->data[stack->size] = item;
    stack->size++;
}

type_t stack_pop(stack_slice_t *stack) {
    if (stack->size == 0) {
        fprintf(stderr, "Stack is empty\n");
        exit(EXIT_FAILURE);
    }

    stack->size--;
    if (!stack->is_on_stack && stack->size > 0 && stack->capacity >= stack->size * STACK_SHRINK_THRESHOLD) {
        size_t new_cap = stack->size * STACK_SHRINK_FACTOR;
        if (new_cap < STACK_MIN_CAPACITY) return stack->data[stack->size];
        type_t *new_data = malloc(new_cap * sizeof(type_t));
        if (!new_data) {
            fprintf(stderr, "Failed to allocate memory for stack\n");
            exit(EXIT_FAILURE);
        }

        memcpy(new_data, stack->data, stack->size * sizeof(type_t));
        free(stack->data);
        stack->data = new_data;
        stack->capacity = new_cap;
    }

    return stack->data[stack->size];
}

void stack_free(stack_slice_t *stack) {
    if (!stack->is_on_stack && stack->data) free(stack->data);
    stack->data = NULL;
    stack->size = 0;
    stack->capacity = 0;
}

