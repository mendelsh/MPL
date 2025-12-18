
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

void vm_run(vm_t *vm, block_t *block) {
    size_t ip = 0;
    while (ip < block->instruction_size) {
        type_t locals[block->local_count];
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

            case CALL_C_FUNC: {
                BuiltinFunc func = read_i32(block->instructions, &ip);
                int argc = read_i32(block->instructions, &ip);
                type_t *argv = &vm->stack.data[vm->stack.size - argc];
                type_t result = builtin_func(func, argc, argv);
                while (argc--) stack_pop(&vm->stack);
                stack_push(&vm->stack, result);
                break;
            }

            default: {
                fprintf(stderr, "Unknown opcode: %d\n", opcode);
                exit(EXIT_FAILURE);
            }
        }
    }
}
