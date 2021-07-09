#ifndef CYNCH_CHUNK_H
#define CYNCH_CHUNK_H

#include "common.h"
#include "value.h"

// List of instructions
typedef enum {
	OP_CONSTANT,
	OP_CONSTANT_LONG,
	OP_ADD,
	OP_SUBTRACT,
	OP_MULTIPLY,
	OP_DIVIDE,
	OP_NEGATE,
	OP_RETURN,
} OpCode;

typedef struct {
	int offset;
	int line;
} LineStart;

// A sequence of bytcode, stored in a dynamic array
typedef struct {
	int count;              // Number of data elements
	int capacity;           // Max size of the array
	uint8_t* code;          // Pointer to the array
	int lineCount;
	int lineCapacity;
	LineStart* lines;       // Source lines contained by the chunk
	ValueArray constants;   // Values contained by the chunk
} Chunk;

void initChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
int addConstant(Chunk* chunk, Value value);
void writeConstant(Chunk* chunk, Value value, int line);
void freeChunk(Chunk* chunk);
int getLine(Chunk* chunk, int instruction);

#endif //CYNCH_CHUNK_H
