#ifndef CYNCH_DEBUG_H
#define CYNCH_DEBUG_H

#include "chunk.h"

void disassembleChunk(Chunk* chunk, const char* name);
int disassembleInstruction(Chunk* chunk, int offset);
static int simpleInstruction(const char* name, int offset);
static int constantInstruction(const char* name, Chunk* chunk, int offset);
static int longConstantInstruction(const char* name, Chunk* chunk, int offset);

#endif //CYNCH_DEBUG_H