#ifndef STACK_H
#define STACK_H

#include "builtin.h"
#include <stdbool.h>

typedef struct {
    bool is_on_stack;
    type_t *data;
    size_t size;
    size_t capacity;
} stack_slice_t;

void stack_init(stack_slice_t *stack, size_t capacity, type_t *buff);
void stack_push(stack_slice_t *stack, type_t item);
type_t stack_pop(stack_slice_t *stack);
void stack_free(stack_slice_t *stack);



#endif // STACK_H
