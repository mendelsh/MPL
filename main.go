package main

import (
	"fmt"
	"sync"

	. "github.com/mendelsh/MPL/builtins"
	. "github.com/mendelsh/MPL/bytecode"
	"github.com/mendelsh/MPL/vm"
)

func main() {
	var v vm.VM
	var wg sync.WaitGroup
	v.WG = &wg

	var helloWorld = vm.Block{
		Instructions: []byte{
			OP_PUSH_CONST, 0,
			OP_CALL_BUILTIN, BF_PRINT, 1,
			OP_HALT,
		},
		Constants: []any{
			"Hello, World!",
		},
	}

	var sleep = vm.Block{
		Instructions: []byte{
			OP_PUSH_CONST, 0,
			OP_CALL_BUILTIN, BF_SLEEP, 1,
			OP_PUSH_CONST, 1,
			OP_CALL_BUILTIN, BF_PRINT, 1,
			OP_HALT,
		},
		Constants: []any{
			1000, // Sleep for 1000 milliseconds
			"Slept for 1 second!",
		},
	}

	var operationAndWorker = vm.Block{
		Instructions: []byte{
			OP_START_WORKER, 0,
			OP_PUSH_CONST, 1,
			OP_PUSH_CONST, 2,
			OP_CALL_OPERATION, BO_ADD,
			OP_CALL_BUILTIN, BF_PRINT, 1,
			OP_HALT,
		},
		Constants: []any{
			&vm.Block{
				Instructions: []byte{
					OP_PUSH_CONST, 0,
					OP_CALL_BUILTIN, BF_PRINT, 1,
					OP_HALT,
				},
				Constants: []any{
					"Hello from worker!",
				},
			},
			10.0,
			20.0,
		},
	}

	// Condition test:
	// if x < 10 ->
	// 	 print("x < 10")
	// else ->
	//   print("x >= 10")

	var conditionTest = vm.Block{
		NumLocals: 1,
		Instructions: []byte{
			OP_PUSH_CONST, 0,
			OP_STORE_LOCAL, 0,

			OP_PUSH_LOCAL, 0,
			OP_PUSH_CONST, 1,
			OP_CALL_OPERATION, BO_LESS_THAN,
			// 10: OP_JUMP_IF_FALSE -> ELSE_START (25)
			OP_JUMP_IF_FALSE, 25, 0, 0, 0,

			// 15: THEN block
			OP_PUSH_CONST, 2,
			OP_CALL_BUILTIN, BF_PRINT, 1,

			// 20: OP_JUMP -> END (30)
			OP_JUMP, 30, 0, 0, 0,

			// 25: ELSE block
			OP_PUSH_CONST, 3,
			OP_CALL_BUILTIN, BF_PRINT, 1,

			OP_HALT,
		},
		Constants: []any{
			5.0,       // 0
			10.0,      // 1
			"x < 10",  // 2
			"x >= 10", // 3
		},
	}

	// Loop test:
	// repeat i = 0; i < 5; i++ ->
	//    print(i)

	var loopTest = vm.Block{
		NumLocals: 1,
		Instructions: []byte{
			OP_PUSH_CONST, 0,
			// 2: OP_STORE_LOCAL 0	; i = 0
			OP_STORE_LOCAL, 0,

			// 4: LOOP START
			OP_PUSH_LOCAL, 0,
			// OP_PUSH_CONST 1  ; 5.0
			OP_PUSH_CONST, 1,
			OP_CALL_OPERATION, BO_LESS_THAN,

			// OP_JUMP_IF_FALSE -> HALT at 33
			OP_JUMP_IF_FALSE, 33, 0, 0, 0,

			// print(i)
			OP_PUSH_LOCAL, 0,
			OP_CALL_BUILTIN, BF_PRINT, 1,

			// i = i + 1
			OP_PUSH_LOCAL, 0,
			OP_PUSH_CONST, 2, // 1.0
			OP_CALL_OPERATION, BO_ADD,
			OP_STORE_LOCAL, 0,

			// Jump back to LOOP START (4)
			OP_JUMP, 4, 0, 0, 0,

			// 33: HALT
			byte(OP_HALT),
		},
		Constants: []any{
			0.0, // 0
			5.0, // 1
			1.0, // 2
		},
	}

	fmt.Println("==== Hello World example ====")
	v.Run(&helloWorld)

	fmt.Println("==== Sleep example ====")
	v.Run(&sleep)

	fmt.Println("==== Operation and Worker example ====")
	v.Run(&operationAndWorker)

	wg.Wait()

	fmt.Println("==== Condition Test ====")
	v.Run(&conditionTest)

	fmt.Println("==== Loop Test ====")
	v.Run(&loopTest)
}
