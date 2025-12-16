package compiler

import "fmt"

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

	// Identifiers
	TOKEN_IDENTIFIER

	// Binary operators
	TOKEN_BINARY_OPERATO
	TOKEN_ASSIGN        // =
	TOKEN_PLUS          // +
	TOKEN_MINUS         // -
	TOKEN_ASTERISK      // *
	TOKEN_POWER         // **
	TOKEN_SLASH         // /
	TOKEN_PERCENT       // %
	TOKEN_EQUAL         // ==
	TOKEN_NOT_EQUAL     // !=
	TOKEN_LESS_THAN     // <
	TOKEN_GREATER_THAN  // >
	TOKEN_LESS_EQUAL    // <=
	TOKEN_GREATER_EQUAL // >=
	TOKEN_AND           // and
	TOKEN_OR            // or
	TOKEN_BITWISE_AND   // &
	TOKEN_BITWISE_OR    // |
	TOKEN_BITWISE_XOR   // ^
	TOKEN_LEFT_SHIFT    // <<
	TOKEN_RIGHT_SHIFT   // >>
	TOKEN_BINARY_OPERATO_END

	// unary operators
	TOKEN_UNARY_OPERATO
	TOKEN_NOT         // not
	TOKEN_BITWISE_NOT // ~
	TOKEN_PLUS_PLUS   // ++
	TOKEN_MINUS_MINUS // --
	TOKEN_UNARY_OPERATO_END

	// Delimiters
	TOKEN_DELIMITER
	TOKEN_COMMA  // ,
	TOKEN_LPAREN // (
	TOKEN_RPAREN // )
	TOKEN_ARROW  // ->
	TOKEN_DELIMITER_END

	// Others
	TOKEN_DOT
	TOKEN_NEWLINE
	TOKEN_INDENT
	TOKEN_DEDENT

	TOKEN_UNKNOWN

	// Built-in functions
	TOKEN_BI_F
	TOKEN_PRINT // print
	TOKEN_SLEEP // sleep
	TOKEN_BI_F_END

	TOKEN_LITERAL
	TOKEN_NUMBER
	TOKEN_STRING
	TOKEN_BOOLEAN
	TOKEN_LITERAL_END
)

var tokensStr = [...]string{
	TOKEN_EOF: "EOF",
	// TOKEN_KEYWORD:            "KEYWORD",
	TOKEN_LET:    "let",
	TOKEN_IF:     "if",
	TOKEN_ELSE:   "else",
	TOKEN_REPEAT: "repeat",
	TOKEN_FUNC:   "func",
	TOKEN_RETURN: "return",
	// TOKEN_KEYWORD_END:        "KEYWORD_END",
	// TOKEN_IDENTIFIER: "identifier",
	// TOKEN_BINARY_OPERATO:     "BINARY_OPERATOR_START",
	TOKEN_ASSIGN:        "=",
	TOKEN_PLUS:          "+",
	TOKEN_MINUS:         "-",
	TOKEN_ASTERISK:      "*",
	TOKEN_POWER:         "**",
	TOKEN_SLASH:         "/",
	TOKEN_PERCENT:       "%",
	TOKEN_EQUAL:         "==",
	TOKEN_NOT_EQUAL:     "!=",
	TOKEN_LESS_THAN:     "<",
	TOKEN_GREATER_THAN:  ">",
	TOKEN_LESS_EQUAL:    "<=",
	TOKEN_GREATER_EQUAL: ">=",
	TOKEN_AND:           "and",
	TOKEN_OR:            "or",
	TOKEN_BITWISE_AND:   "&",
	TOKEN_BITWISE_OR:    "|",
	TOKEN_BITWISE_XOR:   "^",
	TOKEN_LEFT_SHIFT:    "<<",
	TOKEN_RIGHT_SHIFT:   ">>",
	// TOKEN_BINARY_OPERATO_END: "BINARY_OPERATOR_END",
	// TOKEN_UNARY_OPERATO:      "UNARY_OPERATOR_START",
	TOKEN_NOT:         "not",
	TOKEN_BITWISE_NOT: "~",
	TOKEN_PLUS_PLUS:   "++",
	TOKEN_MINUS_MINUS: "--",
	// TOKEN_UNARY_OPERATO_END:  "UNARY_OPERATOR_END",
	// TOKEN_DELIMITER:          "DELIMITER",
	TOKEN_COMMA:  ",",
	TOKEN_LPAREN: "(",
	TOKEN_RPAREN: ")",
	TOKEN_ARROW:  "->",
	// TOKEN_DELIMITER_END:      "DELIMITER_END",
	TOKEN_DOT: ".",
	// TOKEN_NEWLINE:            "NEWLINE",
	TOKEN_INDENT:  "INDENT",
	TOKEN_DEDENT:  "DEDENT",
	TOKEN_UNKNOWN: "UNKNOWN",
	// TOKEN_BI_F:               "BUILTIN_FUNCTIONS",
	TOKEN_PRINT: "print",
	TOKEN_SLEEP: "sleep",
	// TOKEN_BI_F_END:           "BUILTIN_FUNCTIONS_END",
	// TOKEN_LITERAL:            "LITERAL",
	// TOKEN_NUMBER:  "number",
	// TOKEN_STRING:  "string",
	// TOKEN_BOOLEAN: "boolean",
	// TOKEN_LITERAL_END:        "LITERAL_END",
}

type Token struct {
	Type  TokenType
	Value any
}

func (t Token) String() string {
	if t.Type < TOKEN_LITERAL && TOKEN_LITERAL_END > t.Type || t.Type == TOKEN_IDENTIFIER {
		switch t.Type {

		case TOKEN_NUMBER:
			return fmt.Sprintf("%v", t.Value)

		case TOKEN_STRING, TOKEN_IDENTIFIER:
			return fmt.Sprintf("%q", t.Value)

		case TOKEN_BOOLEAN:
			if t.Value.(bool) {
				return "true"
			}
			return "false"

		default:
			return tokensStr[TOKEN_UNKNOWN]
		}
	}

	return tokensStr[t.Type]
}

/// Global map for managing identifiers to the 
var identifiers map[string]int 


