package vm

import (
	"sync"

	"github.com/mendelsh/MPL/builtins"
	. "github.com/mendelsh/MPL/bytecode"
)

///
///		vm.go
/// 	This file contains the implementation of the virtual machine for the MPL programming language.
///
///
/// 	this is a simple first implementation and may be extended or change in the future
///
/// 	we will start with a simple virtual machine that can execute bytecode instructions
///		the general structure is:
///
///			BYTECODE1 arg1 arg2...
///			BYTECODE2 arg1 arg2...
///			.
///			.
///			.
///			BYTECODEN arg1 arg2...
///
///			Args[arg1, arg2...]
///			Constants[const1, const2...]

type Block struct {
	Instructions []byte
	Args         []int
	Constants    []any
	Locals       []any
	NumLocals    int
}

type VM struct {
	WG *sync.WaitGroup
}

// this function is used to convert a 4 byte array to an integer
// might be used for passing arguments directly in the bytecode
func bytes4ToInt(b []byte) int {
	return int(uint(b[0]) | uint(b[1])<<8 | uint(b[2])<<16 | uint(b[3])<<24)
}

func (v VM) Run(block *Block) any {
	ip := 0
	argip := 0
	stack := []any{}
	var locals []any
	if block.Locals != nil {
		locals = block.Locals
	} else {
		locals = make([]any, block.NumLocals)
	}
	instructions := block.Instructions
	args := block.Args
	constants := block.Constants

	for ip < len(instructions) {
		op := instructions[ip]

		switch op {

		case OP_HALT:
			return nil

		case OP_PUSH_CONST:
			stack = append(stack, constants[args[argip]])
			ip++
			argip++

		case OP_PUSH_LOCAL:
			stack = append(stack, locals[args[ip]])
			ip++
			argip++

		case OP_STORE_LOCAL:
			locals[args[ip]] = stack[len(stack)-1]
			stack = stack[:len(stack)-1]
			ip++
			argip++

		case OP_POP:
			stack = stack[:len(stack)-1]

		// TODO: make it a swich case inline operations to reduce function calls
		case OP_CALL_OPERATION:
			operation := builtins.Operations[args[argip]]
			res := operation(stack)
			if args[ip] > builtins.BO_unary {
				stack = stack[:len(stack)-1]
			} else {
				stack = stack[:len(stack)-2]
			}
			stack = append(stack, res)
			ip++
			argip++

		case OP_CALL_BUILTIN:
			builtin := builtins.BuiltinFunctions[args[argip]]
			ip++
			numArgs := args[argip+1]
			callArgs := stack[len(stack)-numArgs:]
			stack = stack[:len(stack)-numArgs]
			stack = append(stack, builtin(callArgs...))
			argip += 2

		case OP_START_WORKER:
			worker := constants[args[argip]].(*Block)
			if v.WG != nil {
				v.WG.Add(1)
				go func(b *Block) {
					defer v.WG.Done()
					v.Run(b)
				}(worker)
			} else {
				go v.Run(worker)
			}
			ip++
			argip++

		case OP_JUMP:
			ip = args[ip]

		case OP_JUMP_IF_FALSE:
			if !stack[len(stack)-1].(bool) {
				ip = args[argip]
			} else {
				ip++
			}
			stack = stack[:len(stack)-1]

		case OP_JUMP_IF_TRUE:
			if stack[len(stack)-1].(bool) {
				ip = args[ip]
			} else {
				ip++
			}
			stack = stack[:len(stack)-1]

		case OP_CALL_FUNCTION:
			fn := constants[args[ip]].(*Block)
			ip++
			numArgs := args[ip]
			ip++

			callArgs := stack[len(stack)-numArgs:]
			stack = stack[:len(stack)-numArgs]

			locals := make([]any, fn.NumLocals)
			copy(locals, callArgs)

			result := v.Run(&Block{
				Instructions: fn.Instructions,
				Args:         fn.Args,
				Constants:    fn.Constants,
				NumLocals:    fn.NumLocals,
				Locals:       locals,
			})

			stack = append(stack, result)

		case OP_RETURN:
			return stack[len(stack)-1]

		default:
			panic("unknown opcode")
		}
	}

	return nil
}
