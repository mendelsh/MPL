package vm

import (
	"math"
	"math/big"
	"sync"

	"github.com/mendelsh/MPL/builtins"
	"github.com/mendelsh/MPL/bytecode"
)

type Block struct {
	Instructions []byte
	Constants    []any
	NumLocals    int
	localsFn     []any
}

type VM struct {
	WG *sync.WaitGroup
}

func (v VM) Run(block *Block) any {
	ip := 0
	stack := []any{}
	var locals []any
	if block.localsFn != nil {
		locals = block.localsFn
	} else {
		locals = make([]any, block.NumLocals)
	}
	instructions := block.Instructions
	constants := block.Constants

	for ip < len(instructions) {
		op := instructions[ip]
		ip++

		switch op {

		case bytecode.OP_HALT:
			return nil

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

		case bytecode.OP_CALL_OPERATION:
			o := int(instructions[ip])
			ip++
			switch o {

			case builtins.BO_ADD:
				b := stack[len(stack)-1].(float64)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(float64)
				stack = stack[:len(stack)-1]
				stack = append(stack, a+b)

			case builtins.BO_SUBTRACT:
				b := stack[len(stack)-1].(float64)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(float64)
				stack = stack[:len(stack)-1]
				stack = append(stack, a-b)

			case builtins.BO_MULTIPLY:
				b := stack[len(stack)-1].(float64)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(float64)
				stack = stack[:len(stack)-1]
				stack = append(stack, a*b)

			case builtins.BO_DIVIDE:
				b := stack[len(stack)-1].(float64)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(float64)
				stack = stack[:len(stack)-1]
				stack = append(stack, a/b)

			case builtins.BO_MODULO:
				b := stack[len(stack)-1].(int)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(int)
				stack = stack[:len(stack)-1]
				stack = append(stack, a%b)

			case builtins.BO_POWER:
				b := stack[len(stack)-1].(float64)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(float64)
				stack = stack[:len(stack)-1]
				stack = append(stack, math.Pow(a, b))

			case builtins.BO_EQUAL:
				b := stack[len(stack)-1]
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1]
				stack = stack[:len(stack)-1]

				// TODO: improve equality checks
				stack = append(stack, a == b)

			case builtins.BO_NOT_EQUAL:
				b := stack[len(stack)-1]
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1]
				stack = stack[:len(stack)-1]

				stack = append(stack, !(a == b))

			case builtins.BO_LESS_THAN:
				b := stack[len(stack)-1].(float64)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(float64)
				stack = stack[:len(stack)-1]
				stack = append(stack, a < b)

			case builtins.BO_GREATER_THAN:
				b := stack[len(stack)-1].(float64)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(float64)
				stack = stack[:len(stack)-1]
				stack = append(stack, a > b)

			case builtins.BO_LESS_EQUAL:
				b := stack[len(stack)-1].(float64)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(float64)
				stack = stack[:len(stack)-1]
				stack = append(stack, a <= b)

			case builtins.BO_GREATER_EQUAL:
				b := stack[len(stack)-1].(float64)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(float64)
				stack = stack[:len(stack)-1]
				stack = append(stack, a >= b)

			case builtins.BO_AND:
				b := stack[len(stack)-1].(bool)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(bool)
				stack = stack[:len(stack)-1]
				stack = append(stack, a && b)

			case builtins.BO_OR:
				b := stack[len(stack)-1].(bool)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(bool)
				stack = stack[:len(stack)-1]
				stack = append(stack, a || b)

			case builtins.BO_NOT:
				a := stack[len(stack)-1].(bool)
				stack = stack[:len(stack)-1]
				stack = append(stack, !a)

			case builtins.BO_BITWISE_AND:
				b := stack[len(stack)-1].(int)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(int)
				stack = stack[:len(stack)-1]
				stack = append(stack, a&b)

			case builtins.BO_BITWISE_OR:
				b := stack[len(stack)-1].(int)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(int)
				stack = stack[:len(stack)-1]
				stack = append(stack, a|b)

			case builtins.BO_BITWISE_XOR:
				b := stack[len(stack)-1].(int)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(int)
				stack = stack[:len(stack)-1]
				stack = append(stack, a^b)

			case builtins.BO_BITWISE_NOT:
				a := stack[len(stack)-1].(int)
				stack = stack[:len(stack)-1]
				stack = append(stack, ^a)

			case builtins.BO_LEFT_SHIFT:
				b := stack[len(stack)-1].(uint)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(int)
				stack = stack[:len(stack)-1]
				stack = append(stack, a<<b)

			case builtins.BO_RIGHT_SHIFT:
				b := stack[len(stack)-1].(uint)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(int)
				stack = stack[:len(stack)-1]
				stack = append(stack, a>>b)

			case builtins.BO_BIG_ADD:
				b := stack[len(stack)-1].(big.Float)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(big.Float)
				stack = stack[:len(stack)-1]
				var result big.Float
				result.Add(&a, &b)
				stack = append(stack, result)

			case builtins.BO_BIG_SUBTRACT:
				b := stack[len(stack)-1].(big.Float)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(big.Float)
				stack = stack[:len(stack)-1]
				var result big.Float
				result.Sub(&a, &b)
				stack = append(stack, result)

			case builtins.BO_BIG_MULTIPLY:
				b := stack[len(stack)-1].(big.Float)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(big.Float)
				stack = stack[:len(stack)-1]
				var result big.Float
				result.Mul(&a, &b)
				stack = append(stack, result)

			case builtins.BO_BIG_DIVIDE:
				b := stack[len(stack)-1].(big.Float)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(big.Float)
				stack = stack[:len(stack)-1]
				var result big.Float
				result.Quo(&a, &b)
				stack = append(stack, result)

			case builtins.BO_BIG_MODULO:
				b := stack[len(stack)-1].(big.Int)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(big.Int)
				stack = stack[:len(stack)-1]
				var result big.Int
				result.Mod(&a, &b)
				stack = append(stack, result)

			case builtins.BO_BIG_POWER:
				// TODO: implement big power
				panic("big power not implemented")

			case builtins.BO_BIG_EQUAL:
				b := stack[len(stack)-1].(big.Float)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(big.Float)
				stack = stack[:len(stack)-1]
				stack = append(stack, a.Cmp(&b) == 0)

			case builtins.BO_BIG_NOT_EQUAL:
				b := stack[len(stack)-1].(big.Float)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(big.Float)
				stack = stack[:len(stack)-1]
				stack = append(stack, a.Cmp(&b) != 0)

			case builtins.BO_BIG_LESS_THAN:
				b := stack[len(stack)-1].(big.Float)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(big.Float)
				stack = stack[:len(stack)-1]
				stack = append(stack, a.Cmp(&b) == -1)

			case builtins.BO_BIG_GREATER_THAN:
				b := stack[len(stack)-1].(big.Float)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(big.Float)
				stack = stack[:len(stack)-1]
				stack = append(stack, a.Cmp(&b) == 1)

			case builtins.BO_BIG_LESS_EQUAL:
				b := stack[len(stack)-1].(big.Float)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(big.Float)
				stack = stack[:len(stack)-1]
				cmp := a.Cmp(&b)
				stack = append(stack, cmp == -1 || cmp == 0)

			case builtins.BO_BIG_GREATER_EQUAL:
				b := stack[len(stack)-1].(big.Float)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(big.Float)
				stack = stack[:len(stack)-1]
				cmp := a.Cmp(&b)
				stack = append(stack, cmp == 1 || cmp == 0)

			case builtins.BO_BIG_AND:
				b := stack[len(stack)-1].(big.Int)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(big.Int)
				stack = stack[:len(stack)-1]
				var result big.Int
				result.And(&a, &b)
				stack = append(stack, result)

			case builtins.BO_BIG_OR:
				b := stack[len(stack)-1].(big.Int)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(big.Int)
				stack = stack[:len(stack)-1]
				var result big.Int
				result.Or(&a, &b)
				stack = append(stack, result)

			case builtins.BO_BIG_XOR:
				b := stack[len(stack)-1].(big.Int)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(big.Int)
				stack = stack[:len(stack)-1]
				var result big.Int
				result.Xor(&a, &b)
				stack = append(stack, result)

			case builtins.BO_BIG_NOT:
				a := stack[len(stack)-1].(big.Int)
				stack = stack[:len(stack)-1]
				var result big.Int
				result.Not(&a)
				stack = append(stack, result)

			case builtins.BO_BIG_LEFT_SHIFT:
				b := stack[len(stack)-1].(uint)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(big.Int)
				stack = stack[:len(stack)-1]
				var result big.Int
				result.Lsh(&a, b)
				stack = append(stack, result)

			case builtins.BO_BIG_RIGHT_SHIFT:
				b := stack[len(stack)-1].(uint)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(big.Int)
				stack = stack[:len(stack)-1]
				var result big.Int
				result.Rsh(&a, b)
				stack = append(stack, result)

			case builtins.BO_CONCATENATE:
				b := stack[len(stack)-1].(string)
				stack = stack[:len(stack)-1]
				a := stack[len(stack)-1].(string)
				stack = stack[:len(stack)-1]
				stack = append(stack, a+b)

			default:
				panic("unknown builtin operation")
			}

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
				v.WG.Add(1)
				v.WG.Go(func() {
					defer v.WG.Done()
					v.Run(worker)
				})
			} else {
				go v.Run(worker)
			}

		case bytecode.OP_JUMP:
			ip = int(int32(uint32(instructions[ip]) | uint32(instructions[ip+1])<<8 | uint32(instructions[ip+2])<<16 | uint32(instructions[ip+3])<<24))

		case bytecode.OP_JUMP_IF_FALSE:
			if !stack[len(stack)-1].(bool) {
				ip = int(int32(uint32(instructions[ip]) | uint32(instructions[ip+1])<<8 | uint32(instructions[ip+2])<<16 | uint32(instructions[ip+3])<<24))
			} else {
				ip += 4
			}
			stack = stack[:len(stack)-1]

		case bytecode.OP_JUMP_IF_TRUE:
			if stack[len(stack)-1].(bool) {
				ip = int(int32(uint32(instructions[ip]) | uint32(instructions[ip+1])<<8 | uint32(instructions[ip+2])<<16 | uint32(instructions[ip+3])<<24))
			} else {
				ip += 4
			}
			stack = stack[:len(stack)-1]

		case bytecode.OP_CALL_FUNCTION:
			idx := int(instructions[ip])
			ip++
			numArgs := int(instructions[ip])
			ip++

			args := stack[len(stack)-numArgs:]
			stack = stack[:len(stack)-numArgs]

			fnBlock := constants[idx].(*Block)
			fnBlock.localsFn = make([]any, fnBlock.NumLocals)
			copy(fnBlock.localsFn, args)

			result := v.Run(fnBlock)
			if result != nil {
				stack = append(stack, result)
			}

		case bytecode.OP_RETURN:
			if len(stack) > 0 {
				return stack[len(stack)-1]
			} else {
				return nil
			}

		default:
			panic("unknown opcode")
		}
	}

	return nil
}
