#ifndef VM_H
#define VM_H

#include "../data-structures/stack.h"
#include "builtin.h"


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

    PUSH
        [PUSH] [i32 stack_frames_index][i32 local_index]

    STORE
        [STORE] [i32 stack_frames_index][i32 local_index]

    CALL_OP
        [CALL_OP][u8 op]

    JUMP
        [JUMP][i32 offset]

    JUMP_FALSE
        [JUMP_FALSE][i32 offset]

    CALL_C_FUNC
        [CALL_C_FUNC][i32 func_id][i32 argc]

    CALL_FUNC
        [CALL_FUNC][byte (0 for constant 1 for local 2 for global)] [i32 stack_frames_index only if byte == 2] [i32 index][i32 argc]
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
    size_t ip;
    size_t stack_base;
} frame_t;

SLICE_TYPE(stack_slice_t, type_t)
FUNCS_IMPL_INIT_PUSH_POP_FREE(stack, stack_slice_t, type_t)

SLICE_TYPE(frame_slice_t, frame_t)
FUNCS_IMPL_INIT_PUSH_POP_FREE(frame, frame_slice_t, frame_t)

typedef struct /* vm_t */ {
    stack_slice_t stack;
} vm_t;

// for now the vm_run return void,
// in the future it might return int for exit code or a value (like type_t/u for example).
void vm_run(vm_t *vm, block_t *block);



#endif // VM_H

