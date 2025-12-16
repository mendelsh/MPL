package builtins

import (
	"fmt"
	"math"
	"time"
)

type BuiltinFunction func(args ...any) any

const (
	BF_PRINT = iota
	BF_SLEEP
)

var BuiltinFunctions = []BuiltinFunction{
	PrintBuiltin,
	SleepBuiltin,
}

func PrintBuiltin(args ...any) any {
	for _, arg := range args {
		fmt.Println(arg)
	}
	return nil
}

func SleepBuiltin(args ...any) any {
	duration, ok := args[0].(int)
	if !ok {
		return nil
	}

	time.Sleep(time.Duration(duration) * time.Millisecond)
	return nil
}

type BuiltinOperation byte

const (
	BO_ADD = iota
	BO_SUBTRACT
	BO_MULTIPLY
	BO_DIVIDE
	BO_MODULO
	BO_POWER
	BO_EQUAL
	BO_NOT_EQUAL
	BO_LESS_THAN
	BO_GREATER_THAN
	BO_LESS_EQUAL
	BO_GREATER_EQUAL
	BO_AND
	BO_OR
	BO_BITWISE_AND
	BO_BITWISE_OR
	BO_BITWISE_XOR
	BO_LEFT_SHIFT
	BO_RIGHT_SHIFT
	BO_BIG_ADD
	BO_BIG_SUBTRACT
	BO_BIG_MULTIPLY
	BO_BIG_DIVIDE
	BO_BIG_MODULO
	BO_BIG_POWER
	BO_BIG_EQUAL
	BO_BIG_NOT_EQUAL
	BO_BIG_LESS_THAN
	BO_BIG_GREATER_THAN
	BO_BIG_LESS_EQUAL
	BO_BIG_GREATER_EQUAL
	BO_BIG_AND
	BO_BIG_OR
	BO_BIG_XOR
	BO_BIG_LEFT_SHIFT
	BO_BIG_RIGHT_SHIFT
	BO_CONCATENATE
	BO_unary
	BO_NOT
	BO_BITWISE_NOT
	BO_BIG_NOT
)

var Operations = [...]func(stack []any) any{
	BO_ADD: func(stack []any) any {
		return stack[len(stack)-2].(float64) + stack[len(stack)-1].(float64)
	},
	BO_SUBTRACT: func(stack []any) any {
		return stack[len(stack)-2].(float64) - stack[len(stack)-1].(float64)
	},
	BO_MULTIPLY: func(stack []any) any {
		return stack[len(stack)-2].(float64) * stack[len(stack)-1].(float64)
	},
	BO_DIVIDE: func(stack []any) any {
		return stack[len(stack)-2].(float64) / stack[len(stack)-1].(float64)
	},
	BO_MODULO: func(stack []any) any {
		return stack[len(stack)-2].(int) % stack[len(stack)-1].(int)
	},
	BO_POWER: func(stack []any) any {
		return math.Pow(stack[len(stack)-2].(float64), stack[len(stack)-1].(float64))
	},
}