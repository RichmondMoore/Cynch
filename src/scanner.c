#include <stdio.h>
#include <string.h>

#include "include/common.h"
#include "include/scanner.h"

typedef struct {
	const char* start;
	const char* current;
	int line;
} Scanner;

Scanner scanner;

/* Initializes a scanner struct
 *
 *  Params:
 *      source:      the source of the tokens to be scanned
 */
void initScanner(const char* source) {
	scanner.start = source;
	scanner.current = source;
	scanner.line = 1;
}

/* Checks if the given character is a digit
 *
 *  Returns:
 *      True if the character is a digit, false otherwise.
 */
static bool isDigit(char c) {
	return c >= '0' && c <= '9';
}

/* Checks if the given character is a letter or underscore
 *
 *  Returns:
 *      True if the character is a letter or underscore, false otherwise.
 */
static bool isAlpha(char c) {
	return (c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z') ||
			c == '_';
}

/* Checks if the scanner is at the end of the file (last token is NULL)
 *
 *  Returns:
 *      True if at the end of the file, false otherwise.
 */
static bool isAtEnd() {
	return *scanner.current == '\0';
}

/* Advances to the next character in the source
 *
 *  Returns:
 *      The next character in the source.
 */
static char advance() {
	scanner.current++;
	return scanner.current[-1];
}

/* Returns the current character without consuming it
 *
 *  Returns:
 *      Returns the current character in the source
 */
static char peek() {
	return *scanner.current;
}

/* Checks the next character without consuming it
 *
 *  Returns:
 *      Returns the next character in the source
 */
static char peekNext() {
	if (isAtEnd()) return '\0';
	return scanner.current[1];
}

/* Checks if the next character in the source code matches what is expected
 *
 *  Params:
 *      expected:     the expected character
 *
 *  Returns:
 *      True of the next character is expected, false otherwise.
 */
static bool match(char expected) {
	if (isAtEnd()) return false;
	if (*scanner.current != expected) return false;
	scanner.current++;
	return true;
}

/* Creates a token of the given type
 *
 *  Params:
 *      type:       the type of token to create
 *
 *  Returns:
 *      A token of the given type containing data from the scanner.
 */
static Token makeToken(TokenType type) {
	Token token;
	token.type = type;
	token.start = scanner.start;
	token.length = (int)(scanner.current - scanner.start);
	token.line = scanner.line;
	return token;
}

/* Creates an error token
 *
 *  Params:
 *      message:       the error message
 *
 *  Returns:
 *      An error token with information about the error.
 */
static Token errorToken(const char* message) {
	Token token;
	token.type = TOKEN_ERROR;
	token.start = message;
	token.length = (int)strlen(message);
	token.line =scanner.line;
	return token;
}

/* Skips next whitespace characters in the source code
 *
 */
static void skipWhitespace() {
	for (;;) {
		char c = peek();

		switch(c) {
			case ' ':
			case '\r':
			case '\t':
				advance();
				break;
			case '\n':
				scanner.line++;
				advance();
				break;
			case '/':
				if (peekNext() == '/') {
					// Comments go for the entire line
					while (peek() != '\n' && !isAtEnd()) advance();
				} else {
					return;
				}
				break;
			default:
				return;
		}
	}
}

/* Checks a given number of characters to see if they match a reserved identifier
 *
 *  Params:
 *      start:          the first character of the token
 *      length:         how many more characters are in the reserved keyword
 *      rest:           the rest of the letters in the reserved keyword
 *      type:           the TokenType corresponding to the given information
 *
 *  Returns:
 *      Returns the TokenType matching the given keyword information, or a TOKEN_IDENTIFIER.
 */
static TokenType checkKeyword(int start, int length, const char* rest, TokenType type) {
	if (scanner.current - scanner.start == start + length &&
	    memcmp(scanner.start + start, rest, length) == 0) {
		return type;
	}

	return TOKEN_IDENTIFIER;
}

/* Determines the TokenType that matches the identifier
 *
 *  Returns:
 *      Returns the TokenType matching the identifier
 */
static TokenType identifierType() {
	switch (scanner.start[0]) {
		case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
		case 'c': return checkKeyword(1, 4, "lass", TOKEN_CLASS);
		case 'e': return checkKeyword(1, 3, "lse", TOKEN_ELSE);
		case 'f':
			if (scanner.current - scanner.start > 1) {
				switch (scanner.start[1]) {
					case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE);
					case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
					case 'u': return checkKeyword(2, 1, "n", TOKEN_FUN);
				}
			}
			break;
		case 'i': return checkKeyword(1, 1, "f", TOKEN_IF);
		case 'n': return checkKeyword(1, 2, "il", TOKEN_NIL);
		case 'o': return checkKeyword(1, 1, "r", TOKEN_OR);
		case 'p': return checkKeyword(1, 4, "rint", TOKEN_PRINT);
		case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
		case 's': return checkKeyword(1, 4, "uper", TOKEN_SUPER);
		case 't':
			if (scanner.current - scanner.start > 1) {
				switch (scanner.start[1]) {
					case 'h': return checkKeyword(2, 2, "is", TOKEN_THIS);
					case 'r': return checkKeyword(2, 2, "UE", TOKEN_TRUE);
				}
			}
			break;
		case 'v': return checkKeyword(1, 2, "ar", TOKEN_VAR);
		case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);
	}

	return TOKEN_IDENTIFIER;
}

/* Detects an identifier and creates a token of that type
 *
 *  Returns:
 *      A token matching the type of the scanned identifier
 */
static Token identifier() {
	while (isAlpha(peek()) || isDigit(peek())) advance();

	return makeToken(identifierType());
}

/* Creates a number token
 *
 *  Returns:
 *      Returns a TOKEN_NUMBER
 */
static Token number() {
	while (isDigit(peek())) advance();

	// Checks for fractional numbers
	if (peek() == '.' && isDigit(peekNext())) {
		// Consumes the '.'
		advance();

		while (isDigit(peek())) advance();
	}

	return makeToken(TOKEN_NUMBER);
}

/* Creates a string token
 *
 *  Returns:
 *      Returns a TOKEN_STRING
 */
static Token string() {
	while (peek() != '"' && !isAtEnd()) {
		if (peek() == '\n') scanner.line++;
		advance();
	}

	if (isAtEnd()) return errorToken("Unterminated string.");

	// This consumes the closing quote
	advance();
	return makeToken(TOKEN_STRING);
}

/* Scans the next token from the location of the scanner
 *
 *  Returns:
 *      The next token or an error token if something is wrong.
 */
Token scanToken() {
	skipWhitespace();
	scanner.start = scanner.current;

	if (isAtEnd()) return makeToken(TOKEN_EOF);

	char c = advance();
	if (isDigit(c)) return number();

	switch (c) {
		case '(': return makeToken(TOKEN_LEFT_PAREN);
		case ')': return makeToken(TOKEN_RIGHT_PAREN);
		case '{': return makeToken(TOKEN_LEFT_BRACE);
		case '}': return makeToken(TOKEN_RIGHT_BRACE);
		case ';': return makeToken(TOKEN_SEMICOLON);
		case ',': return makeToken(TOKEN_COMMA);
		case '.': return makeToken(TOKEN_DOT);
		case '-': return makeToken(TOKEN_MINUS);
		case '+': return makeToken(TOKEN_PLUS);
		case '/': return makeToken(TOKEN_SLASH);
		case '*': return makeToken(TOKEN_STAR);
		case '!':
			return makeToken(match('=' ? TOKEN_BANG_EQUAL : TOKEN_BANG));
		case '=':
			return makeToken(match('=' ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL));
		case '<':
			return makeToken(match('=' ? TOKEN_LESS_EQUAL : TOKEN_LESS));
		case '>':
			return makeToken(match('=' ? TOKEN_GREATER_EQUAL : TOKEN_GREATER));
		case '"': return string();
	}

	return errorToken("Unexpected character.");
}