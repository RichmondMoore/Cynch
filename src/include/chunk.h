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

// A sequence of bytcode, stored in a dynamic array
typedef struct {
	int count;              // Number of data elements
	int capacity;           // Max size of the array
	uint8_t* code;          // Pointer to the array
	int* lines;             // Source lines contained by the chunk
	ValueArray constants;   // Values contained by the chunk
} Chunk;

void initChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
int addConstant(Chunk* chunk, Value value);
void writeConstant(Chunk* chunk, Value value, int line);
void freeChunk(Chunk* chunk);

#endif //CYNCH_CHUNK_H
