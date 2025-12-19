
#include "vm.h"
#include "stdlib.h"


static inline uint8_t read_u8(uint8_t *code, size_t *ip) {
    return code[(*ip)++];
}

static inline int read_i32(uint8_t *code, size_t *ip) {
    int v = BYTES4_TO_INT(code[*ip], code[*ip+1], code[*ip+2], code[*ip+3]);
    *ip += 4;
    return v;
}

void vm_run(vm_t *vm, block_t *main_block) {
    block_t *block = main_block;
    type_t main_locals[block->local_count];
    size_t main_ip = 0;
    frame_t main_frame = {.block = block, .locals = main_locals, .ip = main_ip};

    frame_slice_t stack_frames;
    frame_init(&stack_frames, block->local_count, NULL);
    frame_push(&stack_frames, main_frame);

    type_t *locals = main_locals;
    size_t ip = main_ip;

    while (ip < block->instruction_size) {
        uint8_t opcode = block->instructions[ip];
        ip++;
        
        switch (opcode) {
            case HALT: {
                return;
            }

            case PUSH_CONST: {
                stack_push(&vm->stack, block->constants[read_i32(block->instructions, &ip)]);
                break;
            }

            case PUSH_LOCAL: {
                stack_push(&vm->stack, locals[read_i32(block->instructions, &ip)]);
                break;
            }

            case STORE_LOCAL: {
                locals[read_i32(block->instructions, &ip)] = stack_pop(&vm->stack);
                break;
            }

            case PUSH: {
                type_t *ptr = stack_frames.data[read_i32(block->instructions, &ip)].locals;
                stack_push(&vm->stack, ptr[read_i32(block->instructions, &ip)]);
                break;
            }

            case STORE: {
                type_t *ptr = stack_frames.data[read_i32(block->instructions, &ip)].locals;
                ptr[read_i32(block->instructions, &ip)] = stack_pop(&vm->stack);
                break;
            }

            case POP: {
                stack_pop(&vm->stack);
                break;
            }

            case CALL_OP: {
                uint8_t op = read_u8(block->instructions, &ip);
                type_t r = stack_pop(&vm->stack);
                type_t result;
                if (op > Op_unary) result = operation_unary(op, r);
                else {
                    type_t l = stack_pop(&vm->stack);
                    result = operation(op, l, r);
                }
                stack_push(&vm->stack, result);
                break;
            }

            case JUMP: {
                ip = read_i32(block->instructions, &ip);
                break;
            }

            case JUMP_FALSE: {
                int target = read_i32(block->instructions, &ip);
                if (!stack_pop(&vm->stack).value.bool_u) ip = target;
                break;
            }

            case CALL_C_FUNC: {
                BuiltinFunc func = read_i32(block->instructions, &ip);
                int argc = read_i32(block->instructions, &ip);
                type_t *argv = &vm->stack.data[vm->stack.size - argc];
                type_t result = builtin_func(func, argc, argv);
                while (argc--) stack_pop(&vm->stack);
                stack_push(&vm->stack, result);
                break;
            }

            case CALL_FUNC: {
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

                while (vm->stack.size - caller->stack_base < func->local_count) stack_push(&vm->stack, (type_t){ .type = NONE });

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

                break;
            }

            case RETURN: {
                frame_pop(&stack_frames);
                int frame_index = stack_frames.size - 1;
                frame_t *caller_frame = &stack_frames.data[frame_index];

                type_t return_value = stack_pop(&vm->stack);
                while (vm->stack.size > caller_frame->stack_base) stack_pop(&vm->stack);
                stack_push(&vm->stack, return_value);

                ip = caller_frame->ip;
                block = caller_frame->block;
                locals = caller_frame->locals;
                break;
            }

            default: {
                fprintf(stderr, "Unknown opcode: %d\n", opcode);
                exit(EXIT_FAILURE);
            }
        }
    }
}
