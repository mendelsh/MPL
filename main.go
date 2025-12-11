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

	fmt.Println("==== Hello World example ====")
	v.Run(&helloWorld)

	fmt.Println("==== Sleep example ====")
	v.Run(&sleep)

	fmt.Println("==== Operation and Worker example ====")
	v.Run(&operationAndWorker)

	wg.Wait()
}

