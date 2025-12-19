#include <stdio.h>
#include "vm/vm.h"

/*

int main() {

    block_t func_block;
    uint8_t func_block_code[] = {
        PUSH_CONST, INT_TO_BYTES4(0),
        PUSH_LOCAL, INT_TO_BYTES4(0),
        CALL_C_FUNC, INT_TO_BYTES4(BF_PRINT), INT_TO_BYTES4(2),
        PUSH_CONST, INT_TO_BYTES4(1),
        HALT,
    };
    type_t func_block_consts[] = {
        (type_t){.type = STRING_LITERAL, .value = {.str_literal_u = "Hello,"}},
        (type_t){.type = NONE, .value = {0}},
    };
    func_block.local_count = 1;
    func_block.instruction_size = sizeof(func_block_code);
    func_block.instructions = func_block_code;
    func_block.constants = func_block_consts;

    type_t frame_locals[1];
    frame_t func_frame = {.block = &func_block, .locals = frame_locals, .ip = 0};
    type_t func_frame_type = {.type = FUNCTION, .value = {.ptr_u = &func_frame}};


    
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
        PUSH_CONST, INT_TO_BYTES4(5),
        CALL_FUNC, BYTE(CF_CONSTANT), INT_TO_BYTES4(4), INT_TO_BYTES4(1),
        CALL_C_FUNC, INT_TO_BYTES4(BF_PRINT), INT_TO_BYTES4(1),
        HALT,
    };
    
    type_t consts[] = {
        (type_t){.type = STRING_LITERAL, .value = {.str_literal_u = "Hello, world!"}}, // 0
        (type_t){.type = NUMBER, .value = {.float_u = 3.141592653589793}},             // 1
        (type_t){.type = NUMBER, .value = {.float_u = 2.718281828459045}},             // 2
        (type_t){.type = STRING_LITERAL, .value = {.str_literal_u = "e ** pi ="}},     // 3

        // function
        func_frame_type,                                                               // 4
        (type_t){.type = STRING_LITERAL, .value = {.str_literal_u = "Mendel"}},        // 5
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

*/

int main() {

    // Fibonacci function
    // fib(n) ->
    //     if n <= 1 -> return n
    //     else -> return fib(n-1) + fib(n-2)
    
    block_t fib_block;
    uint8_t fib_code[] = {
        // if n <= 1
        PUSH_LOCAL, INT_TO_BYTES4(0),           // push n
        PUSH_CONST, INT_TO_BYTES4(0),           // push 1
        CALL_OP, BYTE(OP_LE),                   // n <= 1
        JUMP_FALSE, INT_TO_BYTES4(23),          // if false, jump to else (offset 16)
        
        // return n
        PUSH_LOCAL, INT_TO_BYTES4(0),           // push n as return value
        RETURN,
        
        // else: calculate fib(n-1)
        PUSH_LOCAL, INT_TO_BYTES4(0),           // push n
        PUSH_CONST, INT_TO_BYTES4(0),           // push 1
        CALL_OP, BYTE(OP_SUB),                  // n - 1
        CALL_FUNC, BYTE(CF_CONSTANT), INT_TO_BYTES4(1), INT_TO_BYTES4(1),  // call fib(n-1)

        STORE_LOCAL, INT_TO_BYTES4(1),
        
        // calculate fib(n-2)
        PUSH_LOCAL, INT_TO_BYTES4(0),           // push n
        PUSH_CONST, INT_TO_BYTES4(2),           // push 2
        CALL_OP, BYTE(OP_SUB),                  // n - 2
        CALL_FUNC, BYTE(CF_CONSTANT), INT_TO_BYTES4(1), INT_TO_BYTES4(1),  // call fib(n-2)

        PUSH_LOCAL, INT_TO_BYTES4(1),
        
        // add results
        CALL_OP, BYTE(OP_ADD),                  // fib(n-1) + fib(n-2)
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
        // Test fib(0)
        PUSH_CONST, INT_TO_BYTES4(1),
        PUSH_CONST, INT_TO_BYTES4(2),
        CALL_C_FUNC, INT_TO_BYTES4(BF_PRINT), INT_TO_BYTES4(1),
        PUSH_CONST, INT_TO_BYTES4(2),           // arg: 0
        CALL_FUNC, BYTE(CF_CONSTANT), INT_TO_BYTES4(0), INT_TO_BYTES4(1),
        CALL_C_FUNC, INT_TO_BYTES4(BF_PRINT), INT_TO_BYTES4(1),
        
        // Test fib(1)
        PUSH_CONST, INT_TO_BYTES4(1),
        PUSH_CONST, INT_TO_BYTES4(3),
        CALL_C_FUNC, INT_TO_BYTES4(BF_PRINT), INT_TO_BYTES4(1),
        PUSH_CONST, INT_TO_BYTES4(3),           // arg: 1
        CALL_FUNC, BYTE(CF_CONSTANT), INT_TO_BYTES4(0), INT_TO_BYTES4(1),
        CALL_C_FUNC, INT_TO_BYTES4(BF_PRINT), INT_TO_BYTES4(1),
        
        // Test fib(5)
        PUSH_CONST, INT_TO_BYTES4(1),
        PUSH_CONST, INT_TO_BYTES4(4),
        CALL_C_FUNC, INT_TO_BYTES4(BF_PRINT), INT_TO_BYTES4(1),
        PUSH_CONST, INT_TO_BYTES4(4),           // arg: 5
        CALL_FUNC, BYTE(CF_CONSTANT), INT_TO_BYTES4(0), INT_TO_BYTES4(1),
        CALL_C_FUNC, INT_TO_BYTES4(BF_PRINT), INT_TO_BYTES4(1),
        
        // Test fib(10)
        PUSH_CONST, INT_TO_BYTES4(1),
        PUSH_CONST, INT_TO_BYTES4(5),
        CALL_C_FUNC, INT_TO_BYTES4(BF_PRINT), INT_TO_BYTES4(1),
        PUSH_CONST, INT_TO_BYTES4(5),           // arg: 10
        CALL_FUNC, BYTE(CF_CONSTANT), INT_TO_BYTES4(0), INT_TO_BYTES4(1),
        CALL_C_FUNC, INT_TO_BYTES4(BF_PRINT), INT_TO_BYTES4(1),
        
        HALT,
    };
    
    type_t consts[] = {
        fib_block_type,                                                         // 0
        (type_t){.type = STRING_LITERAL, .value = {.str_literal_u = "fib(0) ="}},  // 1
        (type_t){.type = NUMBER, .value = {.float_u = 0}},                      // 2
        (type_t){.type = NUMBER, .value = {.float_u = 1}},                      // 3
        (type_t){.type = NUMBER, .value = {.float_u = 5}},                      // 4
        (type_t){.type = NUMBER, .value = {.float_u = 10}},                     // 5
    };

    size_t code_size = sizeof(code);
    size_t consts_size = sizeof(consts) / sizeof(type_t);
    size_t local_count = 0;

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

/*
```
Expected output:
```
fib(0) = 0.000000
fib(1) = 1.000000
fib(5) = 5.000000
fib(10) = 55.000000
*/

