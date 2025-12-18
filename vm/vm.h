#ifndef VM_H
#define VM_H

#include "stack.h"


/*
    Bytecode formats:

    HALT
        [HALT]

    PUSH_CONST
        [PUSH_CONST][i32 index]

    PUSH_LOCAL
        [PUSH_LOCAL][i32 index]

    STORE_LOCAL
        [STORE_LOCAL][i32 index]

    CALL_OP
        [CALL_OP][u8 op]

    CALL_C_FUNC
        [CALL_C_FUNC][i32 func_id][i32 argc]
*/


typedef struct /* block_t */ {
    uint8_t *instructions;
    size_t instruction_size;
    type_t *constants;
    size_t constant_count;
    size_t local_count;
} block_t;

typedef struct /* frame_t */ {
    block_t *block;
    type_t *locals;
    size_t return_ip;
} frame_t;

typedef struct /* vm_t */ {
    stack_slice_t stack;
} vm_t;

// for now the vm_run return void,
// in the future it might return int for exit code or a value (like type_t/u for example).
void vm_run(vm_t *vm, block_t *block);



#endif // VM_H

