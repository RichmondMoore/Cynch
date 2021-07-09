#include <stdio.h>
#include <stdlib.h>

#include "include/common.h"
#include "include/compiler.h"
#include "include/scanner.h"

#ifdef DEBUG_PRINT_CODE
#include "include/debug.h"
#endif

typedef struct {
	Token current;
	Token previous;
	bool hadError;
	bool panicMode; // Will eventually be cleared at statement boundaries
} Parser;

// Precedence levels from lowest to highest
typedef enum {
	PREC_NONE,
	PREC_ASSIGNMENT,
	PREC_OR,
	PREC_AND,
	PREC_EQUALITY,
	PREC_COMPARISON,
	PREC_TERM,
	PREC_FACTOR,
	PREC_UNARY,
	PREC_CALL,
	PREC_PRIMARY
} Precedence;

typedef void (*ParseFn)(); // A function pointer for parsing

typedef struct {
	ParseFn prefix;
	ParseFn infix;
	Precedence precedence;
} ParseRule;

Parser parser;
Chunk* compilingChunk;

/* Returns the current chunk being compiled
 *
 */
static Chunk* currentChunk() {
	return compilingChunk;
}

/* Prints information about the error, given the token and a message corresponding to the error type
 *
 */
static void errorAt(Token* token, const char* message) {
	if (parser.panicMode) return;
	parser.panicMode = true;
	fprintf(stderr, "[line %d] Error", token->line);

	if (token->type == TOKEN_EOF) {
		fprintf(stderr, " at end.");
	} else if (token->type == TOKEN_ERROR) {
		// Nothing
	} else {
		fprintf(stderr, " at '%.*s'", token->length, token->start);
	}

	fprintf(stderr, ": %s\n", message);
	parser.hadError = true;
}

/* Indicates that there was an error
 *
 *  Params:
 *      message:        a string that indicates that the error is
 */
static void error(const char* message) {
	errorAt(&parser.previous, message);
}

/* Indicates the error occurred at the current token, calls errorAt()
 *
 */
static void errorAtCurrent(const char* message) {
	errorAt(&parser.current, message);
}

/* Scans the next token
 *
 *  Returns:
 *      Prints an error if a TOKEN_ERROR is encountered (by calling errorAtCurrent()).
 */
static void advance() {
	parser.previous = parser.current;

	for (;;) {
		parser.current = scanToken();
		if (parser.current.type != TOKEN_ERROR) break;

		errorAtCurrent(parser.current.start);
	}
}

/* Reads the next token if it matches a given type
 *
 *  Returns:
 *      Reports an error if the token type does not match what it expected
 */
static void consume(TokenType type, const char* message) {
	if (parser.current.type == type) {
		advance();
		return;
	}

	errorAtCurrent(message);
}

/* Writes a byte to the current chunk
 *
 *  Params:
 *      byte:       the byte to be written to the current chunk (could be opcode or operand)
 */
static void emitByte(uint8_t byte) {
	writeChunk(currentChunk(), byte, parser.previous.line);
}

/* A convenience function to emit two bytes
 *
 */
static void emitBytes(uint8_t byte1, uint8_t byte2) {
	emitByte(byte1);
	emitByte(byte2);
}

/*  Emits a return instruction to the end of the chunk
 *
 */
static void emitReturn() {
	emitByte(OP_RETURN);
}

/* Adds a constant to the current chunk, reports an error if there are too many constants in the chunk
 *
 *  Returns:
 *      A Value constant.
 */
static uint8_t makeConstant(Value value) {
	int constant = addConstant(currentChunk(), value);
	if (constant > UINT8_MAX) {
		error("Too many constants in one chunk.");
		return 0;
	}

	return (uint8_t)constant;
}

/* Compiles a constant value
 *
 */
static void emitConstant(Value value) {
	emitBytes(OP_CONSTANT, makeConstant(value));
}

/* Signals the end of compilation
 *
 */
static void endCompiler() {
	emitReturn();
#ifdef DEBUG_PRINT_CODE
	if (!parser.hadError) {
		disassembleChunk(currentChunk(), "code");
	}
#endif
}

static void expression();
static ParseRule* getRule(TokenType type);
static void parsePrecedence(Precedence precedence);

/* Compiles a binary expression
 *
 */
static void binary() {
	TokenType operatorType = parser.previous.type;
	ParseRule* rule = getRule(operatorType);
	parsePrecedence((Precedence)(rule->precedence + 1));

	switch (operatorType) {
		case TOKEN_PLUS:        emitByte(OP_ADD); break;
		case TOKEN_MINUS:       emitByte(OP_SUBTRACT); break;
		case TOKEN_STAR:        emitByte(OP_MULTIPLY); break;
		case TOKEN_SLASH:       emitByte(OP_DIVIDE); break;
		default: return;
	}
}

/* Compiles a group of parentheses
 *
 */
static void grouping() {
	expression();
	consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

/* Compiles a number literal
 *
 */
static void number() {
	double value = strtod(parser.previous.start, NULL);
	emitConstant(value);
}

/* Compiles a unary operator
 *
 */
static void unary() {
	TokenType operatorType = parser.previous.type;

	// Compile operand
	parsePrecedence(PREC_UNARY);

	// Emit operator instruction
	switch(operatorType) {
		case TOKEN_MINUS: emitByte(OP_NEGATE); break;
		default: return;
	}
}

ParseRule rules[] = {
		[TOKEN_LEFT_PAREN]    = {grouping,  NULL,   PREC_NONE},
		[TOKEN_RIGHT_PAREN]   = {NULL,NULL,   PREC_NONE},
		[TOKEN_LEFT_BRACE]    = {NULL,NULL,   PREC_NONE},
		[TOKEN_RIGHT_BRACE]   = {NULL,NULL,   PREC_NONE},
		[TOKEN_COMMA]         = {NULL,NULL,   PREC_NONE},
		[TOKEN_DOT]           = {NULL,NULL,   PREC_NONE},
		[TOKEN_MINUS]         = {unary,          binary, PREC_TERM},
		[TOKEN_PLUS]          = {NULL,     binary, PREC_TERM},
		[TOKEN_SEMICOLON]     = {NULL,NULL,   PREC_NONE},
		[TOKEN_SLASH]         = {NULL,     binary, PREC_FACTOR},
		[TOKEN_STAR]          = {NULL,     binary, PREC_FACTOR},
		[TOKEN_BANG]          = {NULL,NULL,   PREC_NONE},
		[TOKEN_BANG_EQUAL]    = {NULL,NULL,   PREC_NONE},
		[TOKEN_EQUAL]         = {NULL,NULL,   PREC_NONE},
		[TOKEN_EQUAL_EQUAL]   = {NULL,NULL,   PREC_NONE},
		[TOKEN_GREATER]       = {NULL,NULL,   PREC_NONE},
		[TOKEN_GREATER_EQUAL] = {NULL,NULL,   PREC_NONE},
		[TOKEN_LESS]          = {NULL,NULL,   PREC_NONE},
		[TOKEN_LESS_EQUAL]    = {NULL,NULL,   PREC_NONE},
		[TOKEN_IDENTIFIER]    = {NULL,NULL,   PREC_NONE},
		[TOKEN_STRING]        = {NULL,NULL,   PREC_NONE},
		[TOKEN_NUMBER]        = {number,    NULL,   PREC_NONE},
		[TOKEN_AND]           = {NULL,NULL,   PREC_NONE},
		[TOKEN_CLASS]         = {NULL,NULL,   PREC_NONE},
		[TOKEN_ELSE]          = {NULL,NULL,   PREC_NONE},
		[TOKEN_FALSE]         = {NULL,NULL,   PREC_NONE},
		[TOKEN_FOR]           = {NULL,NULL,   PREC_NONE},
		[TOKEN_FUN]           = {NULL,NULL,   PREC_NONE},
		[TOKEN_IF]            = {NULL,NULL,   PREC_NONE},
		[TOKEN_NIL]           = {NULL,NULL,   PREC_NONE},
		[TOKEN_OR]            = {NULL,NULL,   PREC_NONE},
		[TOKEN_PRINT]         = {NULL,NULL,   PREC_NONE},
		[TOKEN_RETURN]        = {NULL,NULL,   PREC_NONE},
		[TOKEN_SUPER]         = {NULL,NULL,   PREC_NONE},
		[TOKEN_THIS]          = {NULL,NULL,   PREC_NONE},
		[TOKEN_TRUE]          = {NULL,NULL,   PREC_NONE},
		[TOKEN_VAR]           = {NULL,NULL,   PREC_NONE},
		[TOKEN_WHILE]         = {NULL,NULL,   PREC_NONE},
		[TOKEN_ERROR]         = {NULL,NULL,   PREC_NONE},
		[TOKEN_EOF]           = {NULL,NULL,   PREC_NONE},
};

/* Orchestrates the usage of the define parsing functions
 *
 */
static void parsePrecedence(Precedence precedence) {
	advance();
	ParseFn prefixRule = getRule(parser.previous.type)->prefix;
	if (prefixRule == NULL) {
		error("Expect expression.");
		return;
	}

	prefixRule();

	while (precedence <= getRule(parser.current.type)->precedence) {
		advance();
		ParseFn infixRule = getRule(parser.previous.type)->infix;
		infixRule();
	}
}

/* Gets parsing rule for a given token type
 *
 *  Returns:
 *      A pointer to the parsing rule associated with given token type.
 */
static ParseRule* getRule(TokenType type) {
	return &rules[type];
}

/* Compiles expressions
 *
 */
static void expression() {
	parsePrecedence(PREC_ASSIGNMENT);
}

/* Compiles the code from the given source
 *
 *  Params:
 *      source:     file containing the code
 *      chunk:      where to store the corresponding bytecode
 *
 *  Returns:
 *      True if there was no error, false otherwise (indicates a compilation error).
 */
bool compile(const char* source, Chunk* chunk) {
	initScanner(source);
	compilingChunk = chunk;

	parser.hadError = false;
	parser.panicMode = false;

	advance();
	expression();
	consume(TOKEN_EOF, "Expect end of expression.");

	endCompiler();
	return !parser.hadError;
}