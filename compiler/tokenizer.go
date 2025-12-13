package compiler

///		tokenizer.go
/// 	This file contains the implementation of the tokenizer for the MPL programming language.
///
///
/// 	this is a simple first implementation and may be extended or change in the future
///
/// 	we will start with a simple tokenizer that can recognize basic tokens
/// 	this is a simple syntax example:
/// 	let x = 10 + 20
/// 	if x > 15 ->
///    	    print("x is greater than 15")
/// 	else ->
///    		print("x is less than or equal to 15")
/// 	repeat 5 ->
///    		print("Hello, World!")
///    		sleep(1000)
///
/// 	if the block is 1 line, we can do like this:
/// 	if x > 15 -> print("x is greater than 15")
/// 	repeat 5 -> print("Hello, World!")
///
/// 	functions can be defined like this:
/// 	func add(a, b) ->
/// 		return a + b
/// 	and called like this:
/// 	let result = add(10, 20)
///
///

type TokenType int

const (
	TOKEN_EOF TokenType = iota

	// Keywords
	TOKEN_KEYWORD
	TOKEN_LET
	TOKEN_IF
	TOKEN_ELSE
	TOKEN_REPEAT
	TOKEN_FUNC
	TOKEN_RETURN
	TOKEN_KEYWORD_END

	// Identifiers and literals
	TOKEN_IDENTIFIER
	TOKEN_LITERAL
	TOKEN_NUMBER
	TOKEN_STRING
	TOKEN_BOOLEAN
	TOKEN_LITERAL_END

	// Operators
	TOKEN_BINARY_OPERATO
	TOKEN_ASSIGN        // =
	TOKEN_PLUS          // +
	TOKEN_MINUS         // -
	TOKEN_ASTERISK      // *
	TOKEN_SLASH         // /
	TOKEN_PERCENT       // %
	TOKEN_CARET         // ^
	TOKEN_EQUAL         // ==
	TOKEN_NOT_EQUAL     // !=
	TOKEN_LESS_THAN     // <
	TOKEN_GREATER_THAN  // >
	TOKEN_LESS_EQUAL    // <=
	TOKEN_GREATER_EQUAL // >=
	TOKEN_AND           // and
	TOKEN_OR            // or
	// TOKEN_NOT         // not
	TOKEN_BITWISE_AND // &
	TOKEN_BITWISE_OR  // |
	TOKEN_BITWISE_XOR // ^
	// TOKEN_BITWISE_NOT    // ~
	TOKEN_LEFT_SHIFT  // <<
	TOKEN_RIGHT_SHIFT // >>
	TOKEN_BINARY_OPERATO_END

	// Delimiters
	TOKEN_DELIMITER
	TOKEN_COMMA  // ,
	TOKEN_LPAREN // (
	TOKEN_RPAREN // )
	TOKEN_ARROW  // ->
	TOKEN_DELIMITER_END

	// Built-in functions
	TOKEN_BI_F
	TOKEN_PRINT // print
	TOKEN_SLEEP // sleep
	TOKEN_BI_F_END

	// Others
	TOKEN_NEWLINE
	TOKEN_INDENT
	TOKEN_DEDENT

	TOKEN_UNKNOWN
)

type Token struct {
	Type  TokenType
	Value string
}

type Tokenizer struct {
	input        string
	position     int
	readPosition int
	ch           byte
	line         int
	column       int
}

func NewTokenizer(input string) *Tokenizer {
	t := &Tokenizer{input: input, line: 1, column: 0}
	t.readChar()
	return t
}

func (t *Tokenizer) readChar() {
	if t.readPosition >= len(t.input) {
		t.ch = 0
	} else {
		t.ch = t.input[t.readPosition]
	}
	t.position = t.readPosition
	t.readPosition++
	if t.ch == '\n' {
		t.line++
		t.column = 0
	} else {
		t.column++
	}
}

// func (t )
