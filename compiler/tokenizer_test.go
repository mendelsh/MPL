package compiler

// import (
// 	"testing"
// )

// func TestTokenizer(t *testing.T) {
// 	input := `
// let x = 10 + 20 - 5 * 2 / 3 % 2 ^ 4
// if x >= 15 and x <= 30 -> print("x is in range")
// else -> print("x is out of range")
// repeat 3 ->
//     sleep(1000)
//     print("Hello, World!")
// y = 0b1010
// z = 0xFF
// w = 3.14
// unknown@token
// `

// 	tests := []struct {
// 		expectedType  TokenType
// 		expectedValue any
// 		checkValue    bool // only check value if this is true
// 	}{
// 		// First line: empty newline
// 		{TOKEN_NEWLINE, nil, false},

// 		// let x = 10 + 20 - 5 * 2 / 3 % 2 ^ 4
// 		{TOKEN_LET, nil, false},
// 		{TOKEN_IDENTIFIER, "x", true},
// 		{TOKEN_ASSIGN, nil, false},
// 		{TOKEN_NUMBER, float64(10), true},
// 		{TOKEN_PLUS, nil, false},
// 		{TOKEN_NUMBER, float64(20), true},
// 		{TOKEN_MINUS, nil, false},
// 		{TOKEN_NUMBER, float64(5), true},
// 		{TOKEN_ASTERISK, nil, false},
// 		{TOKEN_NUMBER, float64(2), true},
// 		{TOKEN_SLASH, nil, false},
// 		{TOKEN_NUMBER, float64(3), true},
// 		{TOKEN_PERCENT, nil, false},
// 		{TOKEN_NUMBER, float64(2), true},
// 		{TOKEN_BITWISE_XOR, nil, false}, // ^ is BITWISE_XOR not CARET
// 		{TOKEN_NUMBER, float64(4), true},
// 		{TOKEN_NEWLINE, nil, false},

// 		// if x >= 15 and x <= 30 -> print("x is in range")
// 		{TOKEN_IF, nil, false},
// 		{TOKEN_IDENTIFIER, "x", true},
// 		{TOKEN_GREATER_EQUAL, nil, false},
// 		{TOKEN_NUMBER, float64(15), true},
// 		{TOKEN_AND, nil, false},
// 		{TOKEN_IDENTIFIER, "x", true},
// 		{TOKEN_LESS_EQUAL, nil, false},
// 		{TOKEN_NUMBER, float64(30), true},
// 		{TOKEN_ARROW, nil, false},
// 		{TOKEN_PRINT, nil, false},
// 		{TOKEN_LPAREN, nil, false},
// 		{TOKEN_STRING, "x is in range", true},
// 		{TOKEN_RPAREN, nil, false},
// 		{TOKEN_NEWLINE, nil, false},

// 		// else -> print("x is out of range")
// 		{TOKEN_ELSE, nil, false},
// 		{TOKEN_ARROW, nil, false},
// 		{TOKEN_PRINT, nil, false},
// 		{TOKEN_LPAREN, nil, false},
// 		{TOKEN_STRING, "x is out of range", true},
// 		{TOKEN_RPAREN, nil, false},
// 		{TOKEN_NEWLINE, nil, false},

// 		// repeat 3 ->
// 		{TOKEN_REPEAT, nil, false},
// 		{TOKEN_NUMBER, float64(3), true},
// 		{TOKEN_ARROW, nil, false},
// 		{TOKEN_NEWLINE, nil, false},

// 		// indented block
// 		{TOKEN_INDENT, 4, true},
// 		{TOKEN_SLEEP, nil, false},
// 		{TOKEN_LPAREN, nil, false},
// 		{TOKEN_NUMBER, float64(1000), true},
// 		{TOKEN_RPAREN, nil, false},
// 		{TOKEN_NEWLINE, nil, false},

// 		{TOKEN_INDENT, 4, true},
// 		{TOKEN_PRINT, nil, false},
// 		{TOKEN_LPAREN, nil, false},
// 		{TOKEN_STRING, "Hello, World!", true},
// 		{TOKEN_RPAREN, nil, false},
// 		{TOKEN_NEWLINE, nil, false},

// 		// y = 0b1010
// 		{TOKEN_IDENTIFIER, "y", true},
// 		{TOKEN_ASSIGN, nil, false},
// 		{TOKEN_NUMBER, float64(10), true},
// 		{TOKEN_NEWLINE, nil, false},

// 		// z = 0xFF
// 		{TOKEN_IDENTIFIER, "z", true},
// 		{TOKEN_ASSIGN, nil, false},
// 		{TOKEN_NUMBER, float64(255), true},
// 		{TOKEN_NEWLINE, nil, false},

// 		// w = 3.14
// 		{TOKEN_IDENTIFIER, "w", true},
// 		{TOKEN_ASSIGN, nil, false},
// 		{TOKEN_NUMBER, float64(3.14), true},
// 		{TOKEN_NEWLINE, nil, false},

// 		// unknown@token
// 		{TOKEN_IDENTIFIER, "unknown", true},
// 		{TOKEN_UNKNOWN, "@", true},
// 		{TOKEN_IDENTIFIER, "token", true},
// 		{TOKEN_NEWLINE, nil, false},

// 		// EOF
// 		{TOKEN_EOF, nil, false},
// 	}

// 	tokenizer := NewTokenizer(input)

// 	for i, tt := range tests {
// 		tok := tokenizer.NextToken()

// 		if tok.Type != tt.expectedType {
// 			t.Fatalf("[%d] token type wrong. expected=%v (%d), got=%v (%d)", 
// 				i, tt.expectedType, tt.expectedType, tok.Type, tok.Type)
// 		}

// 		// Check value only if checkValue is true
// 		if tt.checkValue {
// 			if tok.Value != tt.expectedValue {
// 				t.Fatalf("[%d] token value wrong. expected=%v, got=%v", 
// 					i, tt.expectedValue, tok.Value)
// 			}
// 		}
// 	}
// }

// func TestTokenizerKeywords(t *testing.T) {
// 	input := "let if else repeat func return true false and or not"
	
// 	expected := []TokenType{
// 		TOKEN_LET, TOKEN_IF, TOKEN_ELSE, TOKEN_REPEAT, TOKEN_FUNC, 
// 		TOKEN_RETURN, TOKEN_BOOLEAN, TOKEN_BOOLEAN, TOKEN_AND, TOKEN_OR, TOKEN_NOT,
// 		TOKEN_EOF,
// 	}
	
// 	tokenizer := NewTokenizer(input)
	
// 	for i, expectedType := range expected {
// 		tok := tokenizer.NextToken()
// 		if tok.Type != expectedType {
// 			t.Errorf("token %d: expected type %v, got %v", i, expectedType, tok.Type)
// 		}
// 	}
// }

// func TestTokenizerOperators(t *testing.T) {
// 	input := "+ - * / % ^ == != < > <= >= & | ~ << >>"
	
// 	expected := []TokenType{
// 		TOKEN_PLUS, TOKEN_MINUS, TOKEN_ASTERISK, TOKEN_SLASH, TOKEN_PERCENT,
// 		TOKEN_BITWISE_XOR, TOKEN_EQUAL, TOKEN_NOT_EQUAL, TOKEN_LESS_THAN, 
// 		TOKEN_GREATER_THAN, TOKEN_LESS_EQUAL, TOKEN_GREATER_EQUAL,
// 		TOKEN_BITWISE_AND, TOKEN_BITWISE_OR, TOKEN_BITWISE_NOT,
// 		TOKEN_LEFT_SHIFT, TOKEN_RIGHT_SHIFT, TOKEN_EOF,
// 	}
	
// 	tokenizer := NewTokenizer(input)
	
// 	for i, expectedType := range expected {
// 		tok := tokenizer.NextToken()
// 		if tok.Type != expectedType {
// 			t.Errorf("token %d: expected type %v, got %v", i, expectedType, tok.Type)
// 		}
// 	}
// }

// func TestTokenizerNumbers(t *testing.T) {
// 	tests := []struct {
// 		input    string
// 		expected float64
// 	}{
// 		{"42", 42.0},
// 		{"3.14", 3.14},
// 		{"0b1010", 10.0},
// 		{"0xFF", 255.0},
// 		{"0o77", 63.0},
// 	}
	
// 	for _, tt := range tests {
// 		tokenizer := NewTokenizer(tt.input)
// 		tok := tokenizer.NextToken()
		
// 		if tok.Type != TOKEN_NUMBER {
// 			t.Errorf("input %s: expected TOKEN_NUMBER, got %v", tt.input, tok.Type)
// 			continue
// 		}
		
// 		if tok.Value != tt.expected {
// 			t.Errorf("input %s: expected value %v, got %v", tt.input, tt.expected, tok.Value)
// 		}
// 	}
// }

// func TestTokenizerStrings(t *testing.T) {
// 	input := `"hello world" "test"`
	
// 	tokenizer := NewTokenizer(input)
	
// 	tok1 := tokenizer.NextToken()
// 	if tok1.Type != TOKEN_STRING || tok1.Value != "hello world" {
// 		t.Errorf("first string: expected 'hello world', got %v", tok1.Value)
// 	}
	
// 	tok2 := tokenizer.NextToken()
// 	if tok2.Type != TOKEN_STRING || tok2.Value != "test" {
// 		t.Errorf("second string: expected 'test', got %v", tok2.Value)
// 	}
// }

// func TestTokenizerBooleans(t *testing.T) {
// 	input := "true false"
	
// 	tokenizer := NewTokenizer(input)
	
// 	tok1 := tokenizer.NextToken()
// 	if tok1.Type != TOKEN_BOOLEAN || tok1.Value != true {
// 		t.Errorf("first boolean: expected true, got %v", tok1.Value)
// 	}
	
// 	tok2 := tokenizer.NextToken()
// 	if tok2.Type != TOKEN_BOOLEAN || tok2.Value != false {
// 		t.Errorf("second boolean: expected false, got %v", tok2.Value)
// 	}
// }