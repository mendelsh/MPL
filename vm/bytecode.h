#ifndef BYTECODE_H
#define BYTECODE_H



#include <stdint.h>

#define BYTE(x) ((uint8_t)(x))
#define BYTES4_TO_U32(b1, b2, b3, b4) ((uint32_t)(b1) | ((uint32_t)(b2) << 8) | ((uint32_t)(b3) << 16) | ((uint32_t)(b4) << 24))
#define BYTES4_TO_INT(b1, b2, b3, b4) ((int)BYTES4_TO_U32(b1, b2, b3, b4))
#define INT_TO_BYTES4(x)                        \
    (uint8_t)((uint32_t)(x) & 0xFF),            \
    (uint8_t)(((uint32_t)(x) >> 8) & 0xFF),     \
    (uint8_t)(((uint32_t)(x) >> 16) & 0xFF),    \
    (uint8_t)(((uint32_t)(x) >> 24) & 0xFF)


typedef enum {
    HALT,
    PUSH_CONST,
    PUSH_LOCAL,
    STORE_LOCAL,
    PUSH,
    STORE,
    POP,
    CALL_OP,
    JUMP,
    JUMP_FALSE,
    CALL_C_FUNC,
    CALL_FUNC,
    RETURN,

    INC_LOCAL,
    DEC_LOCAL,

    // might be implemented in the future
    START_WORKER,
} Bytecode;

enum {
    CF_CONSTANT,
    CF_LOCAL,
    CF_GLOBAL,
};



#endif // BYTECODE_H