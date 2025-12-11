package builtins

import "time"

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
		println(arg)
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