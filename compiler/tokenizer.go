package compiler

import (
	"fmt"
	"strconv"
)

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

type Token struct {
	Type  TokenType
	Value any
}

type Tokenizer struct {
	input    string
	position int
	curr_ch  byte
	column   int
}

func NewTokenizer(input string) *Tokenizer {
	return &Tokenizer{
		input:    input,
		position: -1,
		curr_ch:  0,
		column:   -1,
	}
}

func (t *Tokenizer) readChar() bool {
	if t.position+1 >= len(t.input) {
		return false
	}

	t.position++
	t.curr_ch = t.input[t.position]
	t.column++
	if t.curr_ch == '\n' {
		t.column = 0
	}

	return true
}

func (t *Tokenizer) NextToken() Token {
	t.consumeEmptyLines()
	if !t.readChar() {
		return Token{Type: TOKEN_EOF}
	}

	switch t.curr_ch {

	case ' ':
		if t.column != 0 {
			return t.NextToken()
		}

		var i int
		for i = 0; t.readChar() && t.curr_ch == ' '; i++ {
		}
		return Token{Type: TOKEN_INDENT, Value: i}

	case '\n':
		return Token{Type: TOKEN_NEWLINE}

	case '=':
		if t.position+1 < len(t.input) && t.input[t.position+1] == '=' {
			t.readChar()
			return Token{Type: TOKEN_EQUAL}
		}
		return Token{Type: TOKEN_ASSIGN}

	case '+':
		return Token{Type: TOKEN_PLUS}

	case '-':
		if t.position+1 < len(t.input) && t.input[t.position+1] == '>' {
			t.readChar()
			return Token{Type: TOKEN_ARROW}
		}
		return Token{Type: TOKEN_MINUS}

	case '*':
		return Token{Type: TOKEN_ASTERISK}

	case '/':
		return Token{Type: TOKEN_SLASH}

	case '%':
		return Token{Type: TOKEN_PERCENT}

	case '<':
		if t.position+1 < len(t.input) && t.input[t.position+1] == '=' {
			t.readChar()
			return Token{Type: TOKEN_LESS_EQUAL}
		}
		if t.position+1 < len(t.input) && t.input[t.position+1] == '<' {
			t.readChar()
			return Token{Type: TOKEN_LEFT_SHIFT}
		}
		return Token{Type: TOKEN_LESS_THAN}

	case '>':
		if t.position+1 < len(t.input) && t.input[t.position+1] == '=' {
			t.readChar()
			return Token{Type: TOKEN_GREATER_EQUAL}
		}
		if t.position+1 < len(t.input) && t.input[t.position+1] == '>' {
			t.readChar()
			return Token{Type: TOKEN_RIGHT_SHIFT}
		}
		return Token{Type: TOKEN_GREATER_THAN}

	case '!':
		if t.position+1 < len(t.input) && t.input[t.position+1] == '=' {
			t.readChar()
			return Token{Type: TOKEN_NOT_EQUAL}
		}
		return Token{Type: TOKEN_NOT}

	case '&':
		return Token{Type: TOKEN_BITWISE_AND}

	case '|':
		return Token{Type: TOKEN_BITWISE_OR}

	case '^':
		return Token{Type: TOKEN_BITWISE_XOR}

	case '~':
		return Token{Type: TOKEN_BITWISE_NOT}

	case '(':
		return Token{Type: TOKEN_LPAREN}

	case ')':
		return Token{Type: TOKEN_RPAREN}

	case ',':
		return Token{Type: TOKEN_COMMA}

	case '.':
		return Token{Type: TOKEN_DOT}

	case '"':
		p := t.position + 1
		for t.readChar() && t.curr_ch != '"' {
		}
		return Token{Type: TOKEN_STRING, Value: t.input[p:t.position]}
	}

	if !isLetter(t.curr_ch) && !isDigit(t.curr_ch) && t.curr_ch != '_' {
		return Token{Type: TOKEN_UNKNOWN, Value: string(t.curr_ch)}
	}

	w, l := getFirstWord(t.input[t.position:])
	fmt.Printf("DEBUG: w='%s', len=%d, curr_ch='%c'\n", w, l, t.curr_ch)
	t.position += l - 1
	t.curr_ch = t.input[t.position]
	t.column += l - 1

	if num, ok := parseNumber(w); ok {
		return Token{Type: TOKEN_NUMBER, Value: num}
	}

	switch w {

	case "let":
		return Token{Type: TOKEN_LET}

	case "if":
		return Token{Type: TOKEN_IF}

	case "else":
		return Token{Type: TOKEN_ELSE}

	case "repeat":
		return Token{Type: TOKEN_REPEAT}

	case "func":
		return Token{Type: TOKEN_FUNC}

	case "return":
		return Token{Type: TOKEN_RETURN}

	case "true", "false":
		return Token{Type: TOKEN_BOOLEAN, Value: w == "true"}

	case "and":
		return Token{Type: TOKEN_AND}

	case "or":
		return Token{Type: TOKEN_OR}

	case "not":
		return Token{Type: TOKEN_NOT}

	case "print":
		return Token{Type: TOKEN_PRINT}

	case "sleep":
		return Token{Type: TOKEN_SLEEP}

	default:
		return Token{Type: TOKEN_IDENTIFIER, Value: w}
	}
}

// func (t *Tokenizer) consumeEmptyLines() {
// 	position := t.position
// 	curr_ch := t.curr_ch
// 	column := t.column

// 	if !t.readChar() {
// 		return
// 	}

// 	if t.column != 0 || (t.curr_ch != ' ' && t.curr_ch != '\n') {
// 		return
// 	}

// 	for t.readChar() && t.curr_ch == ' ' {
// 	}
// 	if t.curr_ch != '\n' {
// 		t.position = position
// 		t.curr_ch = curr_ch
// 		t.column = column
// 	}

// 	t.consumeEmptyLines()
// }

func (t *Tokenizer) consumeEmptyLines() {
	// if we haven't read a char yet, do nothing
	// NextToken will call readChar() after this
	if t.curr_ch == 0 {
		return
	}

	// if we just returned a newline token, don't consume anything
	// NextToken will read the next character
	if t.curr_ch == '\n' {
		return
	}

	for {
		if t.column != 0 {
			return
		}

		savedPos := t.position
		savedCol := t.column
		savedCh := t.curr_ch

		for t.curr_ch == ' ' {
			if !t.readChar() {
				return
			}
		}

		if t.curr_ch == '\n' {
			if !t.readChar() {
				return
			}
			continue
		}

		t.position = savedPos
		t.column = savedCol
		t.curr_ch = savedCh
		return
	}
}

func getFirstWord(s string) (string, int) {
	for i, r := range s {
		if !(r == '_' ||
			(r >= 'a' && r <= 'z') ||
			(r >= 'A' && r <= 'Z') ||
			(r >= '0' && r <= '9')) {
			return s[:i], i
		}
	}
	return s, len(s)
}

func isLetter(ch byte) bool {
	return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')
}

func isDigit(ch byte) bool {
	return ch >= '0' && ch <= '9'
}

func parseNumber(s string) (float64, bool) {
	if i, err := strconv.ParseInt(s, 0, 64); err == nil {
		return float64(i), true
	}
	if f, err := strconv.ParseFloat(s, 64); err == nil {
		return f, true
	}
	return 0, false
}
