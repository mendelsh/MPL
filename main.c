#include <stdio.h>
#include "vm/vm.h"

int main() {
    
    uint8_t code[] = {
        PUSH_CONST, INT_TO_BYTES4(0),
        CALL_C_FUNC, INT_TO_BYTES4(BF_PRINT), INT_TO_BYTES4(1),
        PUSH_CONST, INT_TO_BYTES4(1),
        PUSH_CONST, INT_TO_BYTES4(2),
        STORE_LOCAL, INT_TO_BYTES4(0),
        STORE_LOCAL, INT_TO_BYTES4(1),
        PUSH_LOCAL, INT_TO_BYTES4(0),
        PUSH_LOCAL, INT_TO_BYTES4(1),
        CALL_OP, BYTE(OP_POW),
        STORE_LOCAL, INT_TO_BYTES4(0),
        PUSH_CONST, INT_TO_BYTES4(3),
        PUSH_LOCAL, INT_TO_BYTES4(0),
        CALL_C_FUNC, INT_TO_BYTES4(BF_PRINT), INT_TO_BYTES4(2),
        HALT,
    };
    
    type_t consts[] = {
        (type_t){.type = STRING_LITERAL, .value = {.str_literal_u = "Hello, world!"}}, // 0
        (type_t){.type = NUMBER, .value = {.float_u = 3.141592653589793}},             // 1
        (type_t){.type = NUMBER, .value = {.float_u = 2.718281828459045}},             // 2
        (type_t){.type = STRING_LITERAL, .value = {.str_literal_u = "e ** pi ="}},     // 3
    };



    size_t code_size = sizeof(code);
    size_t consts_size = sizeof(consts) / sizeof(type_t);
    size_t local_count = 2;

    block_t block = {
        .instruction_size = code_size,
        .instructions = code,
        .constant_count = consts_size,
        .constants = consts,
        .local_count = local_count
    };

    int capacity = 1024;
    type_t buff[capacity];
    stack_slice_t stack;
    stack_init(&stack, capacity, buff);

    vm_t vm = {
        .stack = stack,
    };

    vm_run(&vm, &block);

    return 0;
}
