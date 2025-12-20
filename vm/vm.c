
#include "vm.h"
#include "stdlib.h"


static inline uint8_t read_u8(uint8_t *code, size_t *ip) {
    return code[(*ip)++];
}

static inline int read_i32(uint8_t *code, size_t *ip) {
    int v = BYTES4_TO_INT(code[*ip], code[*ip+1], code[*ip+2], code[*ip+3]);
    // int v = *(int32_t*)&code[*ip];
    *ip += 4;
    return v;
}

void vm_run(vm_t *vm, block_t *main_block) {
    block_t *block = main_block;
    type_t main_locals[block->local_count];
    frame_t main_frame = {.block = block, .locals = main_locals, .ip = 0};

    frame_slice_t stack_frames;
    frame_init(&stack_frames, block->local_count, NULL);
    frame_push(&stack_frames, main_frame);

    type_t *locals = main_locals;
    size_t ip = 0;

    static void *dispatch_table[] = {
        [HALT] = &&op_halt,
        [PUSH_CONST] = &&op_push_const,
        [PUSH_LOCAL] = &&op_push_local,
        [STORE_LOCAL] = &&op_store_local,
        [PUSH] = &&op_push,
        [STORE] = &&op_store,
        [POP] = &&op_pop,
        [CALL_OP] = &&op_call_op,
        [JUMP] = &&op_jump,
        [JUMP_FALSE] = &&op_jump_false,
        [CALL_C_FUNC] = &&op_call_c_func,
        [CALL_FUNC] = &&op_call_func,
        [RETURN] = &&op_return,
        [INC_LOCAL] = &&op_inc_local,
        [DEC_LOCAL] = &&op_dec_local,
    };

    #define DISPATCH() goto *dispatch_table[block->instructions[ip++]]

    DISPATCH();

    op_halt:
        return;

    op_push_const:
        stack_push(&vm->stack, block->constants[read_i32(block->instructions, &ip)]);
        DISPATCH();

    op_push_local:
        stack_push(&vm->stack, locals[read_i32(block->instructions, &ip)]);
        DISPATCH();

    op_store_local:
        locals[read_i32(block->instructions, &ip)] = stack_pop(&vm->stack);
        DISPATCH();

    op_push: {
        type_t *ptr = stack_frames.data[read_i32(block->instructions, &ip)].locals;
        stack_push(&vm->stack, ptr[read_i32(block->instructions, &ip)]);
        DISPATCH();
    }

    op_store: {
        type_t *ptr = stack_frames.data[read_i32(block->instructions, &ip)].locals;
        ptr[read_i32(block->instructions, &ip)] = stack_pop(&vm->stack);
        DISPATCH();
    }

    op_pop:
        stack_pop(&vm->stack);
        DISPATCH();

    op_call_op: {
        uint8_t op = read_u8(block->instructions, &ip);
        type_t r = stack_pop(&vm->stack);
        type_t result;
        if (op > Op_unary) result = operation_unary(op, r);
        else {
            type_t l = stack_pop(&vm->stack);
            result = operation(op, l, r);
        }
        stack_push(&vm->stack, result);
        DISPATCH();
    }

    op_jump:
        ip = read_i32(block->instructions, &ip);
        DISPATCH();

    op_jump_false: {
        int target = read_i32(block->instructions, &ip);
        if (!stack_pop(&vm->stack).value.bool_u) ip = target;
        DISPATCH();
    }

    op_call_c_func: {
        BuiltinFunc func = read_i32(block->instructions, &ip);
        int argc = read_i32(block->instructions, &ip);
        type_t *argv = &vm->stack.data[vm->stack.size - argc];
        type_t result = builtin_func(func, argc, argv);
        while (argc--) stack_pop(&vm->stack);
        stack_push(&vm->stack, result);
        DISPATCH();
    }

    op_call_func: {
        uint8_t func_location = read_u8(block->instructions, &ip);
        type_t obj_type;

        switch (func_location) {
            case CF_CONSTANT:
                obj_type = block->constants[read_i32(block->instructions, &ip)];
                break;
            case CF_LOCAL:
                obj_type = locals[read_i32(block->instructions, &ip)];
                break;
            case CF_GLOBAL: {
                type_t *ptr = stack_frames.data[read_i32(block->instructions, &ip)].locals;
                obj_type = ptr[read_i32(block->instructions, &ip)];
                break;
            }
            default:
                fprintf(stderr, "Unknown function location: %d\n", func_location);
                exit(EXIT_FAILURE);
        }

        block_t *func = obj_type.value.ptr_u;
        int argc = read_i32(block->instructions, &ip);

        frame_t *caller = &stack_frames.data[stack_frames.size - 1];
        caller->ip = ip;
        caller->block = block;
        caller->locals = locals;
        caller->stack_base = vm->stack.size - argc;

        stack_push_n(&vm->stack, func->local_count - argc);

        frame_t callee = {
            .block = func,
            .locals = vm->stack.data + caller->stack_base,
            .ip = 0,
            .stack_base = caller->stack_base
        };

        frame_push(&stack_frames, callee);

        block = func;
        locals = callee.locals;
        ip = 0;
        DISPATCH();
    }

    op_return: {
        frame_pop(&stack_frames);
        int frame_index = stack_frames.size - 1;
        frame_t *caller_frame = &stack_frames.data[frame_index];

        type_t return_value = stack_pop(&vm->stack);
        stack_pop_n(&vm->stack, vm->stack.size - caller_frame->stack_base);
        stack_push(&vm->stack, return_value);

        ip = caller_frame->ip;
        block = caller_frame->block;
        locals = caller_frame->locals;
        DISPATCH();
    }

    op_inc_local:
        locals[read_i32(block->instructions, &ip)].value.float_u += 1;
        DISPATCH();

    op_dec_local:
        locals[read_i32(block->instructions, &ip)].value.float_u -= 1;
        DISPATCH();
}
