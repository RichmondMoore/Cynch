#ifndef CYNCH_CHUNK_H
#define CYNCH_CHUNK_H

#include "common.h"

// List of instructions
typedef enum {
	OP_RETURN,
} OpCode;

// A sequence of bytcode, stored in a dynamic array
typedef struct {
	int count;          // Number of data elements
	int capacity;       // Max size of the array
	uint8_t* code;      // Pointer to the array
} Chunk;

void initChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte);
void freeChunk(Chunk* chunk);

#endif //CYNCH_CHUNK_H
