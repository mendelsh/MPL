package vm

import (
	"sync"
	"github.com/mendelsh/MPL/bytecode"
	"github.com/mendelsh/MPL/builtins"
)

type Block struct {
	Instructions []byte
	Constants    []any
	NumLocals    int
}

type VM struct {
	WG     *sync.WaitGroup
}

func (v VM) Run(block *Block) {
	ip := 0
	stack := []any{}
	locals := make([]any, block.NumLocals)
	instructions := block.Instructions
	constants := block.Constants

	for ip < len(instructions) {
		op := instructions[ip]
		switch op {

		case bytecode.OP_HALT:
			return

		case bytecode.OP_PUSH_CONST:
			idx := int(instructions[ip])
			ip++
			stack = append(stack, constants[idx])

		case bytecode.OP_PUSH_LOCAL:
			idx := int(instructions[ip])
			ip++
			stack = append(stack, locals[idx])

		case bytecode.OP_STORE_LOCAL:
			idx := int(instructions[ip])
			ip++
			val := stack[len(stack)-1]
			stack = stack[:len(stack)-1]
			locals[idx] = val

		case bytecode.OP_POP:
			stack = stack[:len(stack)-1]

		case bytecode.OP_CALL_BUILTIN:
			idx := int(instructions[ip])
			ip++
			numArgs := int(instructions[ip])
			ip++

			args := stack[len(stack)-numArgs:]
			stack = stack[:len(stack)-numArgs]

			result := builtins.BuiltinFunctions[idx](args...)
			if result != nil {
				stack = append(stack, result)
			}

		case bytecode.OP_START_WORKER:
			idx := int(instructions[ip])
			ip++

			worker := block.Constants[idx].(*Block)
			if v.WG != nil {
				v.WG.Go(func() {
					v.Run(worker)
				})
			} else {
				go v.Run(worker)
			}
		
		default:
			panic("unknown opcode")
		}
	}
}
