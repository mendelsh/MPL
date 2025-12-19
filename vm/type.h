#ifndef TYPE_H
#define TYPE_H

#include <stdint.h>
#include <stdbool.h>
#include "bytecode.h"

typedef enum /* Type */ {
    NUMBER,
    STRING,
    STRING_LITERAL,
    BOOL,
    FUNCTION,
    NONE,
    TYPE_T,
} Type;

typedef union /* type_u */ {
    int64_t int_u;
    double float_u;
    bool bool_u;
    void* ptr_u;
    const char* str_literal_u;
} type_u;

typedef struct /* type_t */ {
    Type type;
    type_u value;
} type_t;



#endif // TYPE_H