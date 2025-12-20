#include <stdio.h>
#include "vm/vm.h"

int main() {

    double fib_max = 33;

    // Fibonacci function
    // fib(n) ->
    //     if n <= 1 -> return n
    //     else -> return fib(n-1) + fib(n-2)
    
    block_t fib_block;
    uint8_t fib_code[] = {
        // if n <= 1
        PUSH_LOCAL, INT_TO_BYTES4(0),
        PUSH_CONST, INT_TO_BYTES4(0),
        CALL_OP, BYTE(OP_LE),
        JUMP_FALSE, INT_TO_BYTES4(23),          // if false, jump to else (offset 23)
        
        // return n
        PUSH_LOCAL, INT_TO_BYTES4(0),
        RETURN,
        
        // else: calculate fib(n-1)
        PUSH_LOCAL, INT_TO_BYTES4(0),
        PUSH_CONST, INT_TO_BYTES4(0),
        CALL_OP, BYTE(OP_SUB),
        CALL_FUNC, BYTE(CF_CONSTANT), INT_TO_BYTES4(1), INT_TO_BYTES4(1),  // call fib(n-1)

        STORE_LOCAL, INT_TO_BYTES4(1),
        
        // calculate fib(n-2)
        PUSH_LOCAL, INT_TO_BYTES4(0),
        PUSH_CONST, INT_TO_BYTES4(2),
        CALL_OP, BYTE(OP_SUB),
        CALL_FUNC, BYTE(CF_CONSTANT), INT_TO_BYTES4(1), INT_TO_BYTES4(1),

        PUSH_LOCAL, INT_TO_BYTES4(1),
        
        // fib(n-1) + fib(n-2)
        CALL_OP, BYTE(OP_ADD),
        RETURN,
    };

    type_t fib_block_type = {
        .type = FUNCTION,
        .value = {
            .ptr_u = &fib_block
        }
    };
    
    type_t fib_consts[] = {
        (type_t){.type = NUMBER, .value = {.float_u = 1}},     // 0
        fib_block_type,                                         // 1 (self-reference for recursion)
        (type_t){.type = NUMBER, .value = {.float_u = 2}},     // 2
    };
    
    fib_block.local_count = 2;
    fib_block.instruction_size = sizeof(fib_code);
    fib_block.instructions = fib_code;
    fib_block.constants = fib_consts;
    fib_block.constant_count = sizeof(fib_consts) / sizeof(type_t);

    // Main code
    uint8_t code[] = {
        // fib loop
        
        // initialize n
        PUSH_CONST, INT_TO_BYTES4(2),
        STORE_LOCAL, INT_TO_BYTES4(0),
    
        // loop offset 10
        PUSH_LOCAL, INT_TO_BYTES4(0),
        PUSH_CONST, INT_TO_BYTES4(3),
        CALL_OP, BYTE(OP_LT),
        JUMP_FALSE, INT_TO_BYTES4(86/*offset*/),

        PUSH_LOCAL, INT_TO_BYTES4(0),
        CALL_FUNC, BYTE(CF_CONSTANT), INT_TO_BYTES4(0), INT_TO_BYTES4(1),
        STORE_LOCAL, INT_TO_BYTES4(1),
        PUSH_CONST, INT_TO_BYTES4(1),
        PUSH_LOCAL, INT_TO_BYTES4(0),
        PUSH_CONST, INT_TO_BYTES4(4),
        PUSH_LOCAL, INT_TO_BYTES4(1),
        CALL_C_FUNC, INT_TO_BYTES4(BF_PRINT), INT_TO_BYTES4(4),

        INC_LOCAL, INT_TO_BYTES4(0),
        JUMP, INT_TO_BYTES4(10/*offset*/),
    
        // offset 86
        HALT,
    };
    
    type_t consts[] = {
        fib_block_type,                                                         // 0
        (type_t){.type = STRING_LITERAL, .value = {.str_literal_u = "fib("}},   // 1
        (type_t){.type = NUMBER, .value = {.float_u = 0}},                           // 2
        (type_t){.type = NUMBER, .value = {.float_u = fib_max+1}},              // 3
        (type_t){.type = STRING_LITERAL, .value = {.str_literal_u = ") = "}},   // 4
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

    exit(0);
}

/*
```
Expected output:
```
fib(0) = 0.000000
fib(1) = 1.000000
fib(5) = 5.000000
fib(10) = 55.000000
*/

