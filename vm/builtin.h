#ifndef BUILTIN_H
#define BUILTIN_H

#include "type.h"
#include <math.h>
#include <stdio.h>


/**
 * @enum Op
 * @brief Enumeration of virtual machine bytecode operations
 * 
 * Defines all supported operations for the bytecode interpreter,
 * including arithmetic, comparison, and logical operations.
 * 
 * @var OP_ADD - Addition operation
 * @var OP_SUB - Subtraction operation
 * @var OP_MUL - Multiplication operation
 * @var OP_DIV - Division operation
 * @var OP_MOD - Modulo (remainder) operation
 * @var OP_EQ - Equality comparison operation
 * @var OP_NE - Not-equal comparison operation
 * @var OP_LT - Less-than comparison operation
 * @var OP_GT - Greater-than comparison operation
 * @var OP_LE - Less-than-or-equal comparison operation
 * @var OP_GE - Greater-than-or-equal comparison operation
 * @var OP_AND - Logical AND operation
 * @var OP_OR - Logical OR operation
 * @var OP_NOT - Logical NOT operation
 */
typedef enum /* Op */ {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_POW,
    OP_DIV,
    OP_MOD,
    OP_EQ,
    OP_NE,
    OP_LT,
    OP_GT,
    OP_LE,
    OP_GE,
    OP_AND,
    OP_OR,
    OP_BIT_AND,
    OP_BIT_OR,
    OP_BIT_XOR,
    OP_BIT_SHL,
    OP_BIT_SHR,

    Op_unary,
    OP_NOT,
    OP_BIT_NOT,
} Op;


typedef enum /* BuiltinFunction */ {
    BF_PRINT,
} BuiltinFunc;

/*
#define operation(op, left, right, result)                                                                       \
    static void *dispatch_op_table[] = {                                                                 \
        [OP_ADD] = &&op_add,                                                                             \
        [OP_SUB] = &&op_sub,                                                                             \
        [OP_MUL] = &&op_mul,                                                                             \
        [OP_POW] = &&op_pow,                                                                             \
        [OP_DIV] = &&op_div,                                                                             \
        [OP_MOD] = &&op_mod,                                                                             \
        [OP_EQ] = &&op_eq,                                                                               \
        [OP_NE] = &&op_ne,                                                                               \
        [OP_LT] = &&op_lt,                                                                               \
        [OP_GT] = &&op_gt,                                                                               \
        [OP_LE] = &&op_le,                                                                               \
        [OP_GE] = &&op_ge,                                                                               \
        [OP_AND] = &&op_and,                                                                             \
        [OP_OR] = &&op_or,                                                                               \
        [OP_BIT_AND] = &&op_bit_and,                                                                     \
        [OP_BIT_OR] = &&op_bit_or,                                                                       \
        [OP_BIT_XOR] = &&op_bit_xor,                                                                     \
        [OP_BIT_SHL] = &&op_bit_shl,                                                                     \
        [OP_BIT_SHR] = &&op_bit_shr,                                                                     \
    };                                                                                                   \
    goto *dispatch_op_table[op];                                                                         \
    op_add: result = (type_t){.type = NUMBER, .value.float_u = left.value.float_u + right.value.float_u}; goto end; \
    op_sub: result = (type_t){.type = NUMBER, .value.float_u = left.value.float_u - right.value.float_u}; goto end; \
    op_mul: result = (type_t){.type = NUMBER, .value.float_u = left.value.float_u * right.value.float_u}; goto end; \
    op_pow: result = (type_t){.type = NUMBER, .value.float_u = pow(left.value.float_u, right.value.float_u)}; goto end; \
    op_div: result = (type_t){.type = NUMBER, .value.float_u = left.value.float_u / right.value.float_u}; goto end; \
    op_mod: result = (type_t){.type = NUMBER, .value.float_u = fmod(left.value.float_u, right.value.float_u)}; goto end; \
    op_eq: result = (type_t){.type = BOOL, .value.bool_u = left.value.float_u == right.value.float_u}; goto end; \
    op_ne: result = (type_t){.type = BOOL, .value.bool_u = left.value.float_u != right.value.float_u}; goto end; \
    op_lt: result = (type_t){.type = BOOL, .value.bool_u = left.value.float_u < right.value.float_u}; goto end; \
    op_gt: result = (type_t){.type = BOOL, .value.bool_u = left.value.float_u > right.value.float_u}; goto end; \
    op_le: result = (type_t){.type = BOOL, .value.bool_u = left.value.float_u <= right.value.float_u}; goto end; \
    op_ge: result = (type_t){.type = BOOL, .value.bool_u = left.value.float_u >= right.value.float_u}; goto end; \
    op_and: result = (type_t){.type = BOOL, .value.bool_u = left.value.bool_u && right.value.bool_u}; goto end; \
    op_or: result = (type_t){.type = BOOL, .value.bool_u = left.value.bool_u || right.value.bool_u}; goto end; \
    op_bit_and: result = (type_t){.type = NUMBER, .value.int_u = left.value.int_u & right.value.int_u}; goto end; \
    op_bit_or: result = (type_t){.type = NUMBER, .value.int_u = left.value.int_u | right.value.int_u}; goto end; \
    op_bit_xor: result = (type_t){.type = NUMBER, .value.int_u = left.value.int_u ^ right.value.int_u}; goto end; \
    op_bit_shl: result = (type_t){.type = NUMBER, .value.int_u = left.value.int_u << right.value.int_u}; goto end; \
    op_bit_shr: result = (type_t){.type = NUMBER, .value.int_u = left.value.int_u >> right.value.int_u}; goto end; \
    end:
*/

static inline __attribute__((always_inline)) type_t operation(Op op, type_t left, type_t right) {
    type_t result = {.type = NONE, .value = {0}};

    switch (op) {
        case OP_ADD: result.type = NUMBER; result.value.float_u = left.value.float_u + right.value.float_u; break;
        case OP_SUB: result.type = NUMBER; result.value.float_u = left.value.float_u - right.value.float_u; break;
        case OP_MUL: result.type = NUMBER; result.value.float_u = left.value.float_u * right.value.float_u; break;
        case OP_POW: result.type = NUMBER; result.value.float_u = pow(left.value.float_u, right.value.float_u); break;
        case OP_DIV: result.type = NUMBER; result.value.float_u = left.value.float_u / right.value.float_u; break;
        case OP_MOD: result.type = NUMBER; result.value.int_u = left.value.int_u % right.value.int_u; break;
        case OP_EQ: result.type = BOOL; result.value.bool_u = left.value.float_u == right.value.float_u; break;
        case OP_NE: result.type = BOOL; result.value.bool_u = left.value.float_u != right.value.float_u; break;
        case OP_LT: result.type = BOOL; result.value.bool_u = left.value.float_u < right.value.float_u; break;
        case OP_GT: result.type = BOOL; result.value.bool_u = left.value.float_u > right.value.float_u; break;
        case OP_LE: result.type = BOOL; result.value.bool_u = left.value.float_u <= right.value.float_u; break;
        case OP_GE: result.type = BOOL; result.value.bool_u = left.value.float_u >= right.value.float_u; break;
        case OP_AND: result.type = BOOL; result.value.bool_u = left.value.bool_u && right.value.bool_u; break;
        case OP_OR: result.type = BOOL; result.value.bool_u = left.value.bool_u || right.value.bool_u; break;
        case OP_BIT_AND: result.type = NUMBER; result.value.int_u = left.value.int_u & right.value.int_u; break;
        case OP_BIT_OR: result.type = NUMBER; result.value.int_u = left.value.int_u | right.value.int_u; break;
        case OP_BIT_XOR: result.type = NUMBER; result.value.int_u = left.value.int_u ^ right.value.int_u; break;
        case OP_BIT_SHL: result.type = NUMBER; result.value.int_u = left.value.int_u << right.value.int_u; break;
        case OP_BIT_SHR: result.type = NUMBER; result.value.int_u = left.value.int_u >> right.value.int_u; break;
        default: break;
    }

    return result;
}

static inline type_t operation_unary(Op op, type_t right) {
    type_t result = {.type = NONE, .value = {0}};

    switch (op) {
        case OP_NOT: result.type = BOOL; result.value.bool_u = !right.value.bool_u; break;
        case OP_BIT_NOT: result.type = NUMBER; result.value.int_u = ~right.value.int_u; break;
        default: break;
    }

    return result;
}

static inline type_t builtin_print(int argc, type_t *argv) {
    for (int i = 0; i < argc; i++) {
        switch (argv[i].type) {
            case STRING_LITERAL:
                printf("%s", argv[i].value.str_literal_u);
                break;
            case NUMBER:
                printf("%f", argv[i].value.float_u);
                break;
            case INT:
                printf("%ld", argv[i].value.int_u);
                break;
            case BOOL:
                printf("%s", argv[i].value.bool_u ? "true" : "false");
                break;
            case NONE:
                printf("none");
                break;
            default:
                printf("unknown");
                break;
        }
        // if (i < argc - 1) printf(" ");
    }
    printf("\n");
    return (type_t){.type = NONE, .value = {0}};
}

static inline type_t builtin_func(BuiltinFunc func, int argc, type_t *argv) {
    type_t result = {.type = NONE, .value = {0}};

    switch (func) {
        case BF_PRINT: result = builtin_print(argc, argv); break;
        default: return (type_t){.type = NONE, .value = {0}};
    }

    return result;
}


#endif // BUILTIN_H