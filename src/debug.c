#include <stdio.h>

#include "include/debug.h"

/* Disassembles all of the instructions in a chunk for debugging purposes
 *
 *  Params:
 *      chunk:      the chunk to disassemble
 *      name:       the name of the chunk
 */
void disassembleChunk(Chunk* chunk, const char* name) {
	printf("== %s ==\n", name); // Print a header for the current chunk

	// Disassemble each instruction within the chunk
	for (int offset = 0; offset < chunk->count;) {
		offset = disassembleInstruction(chunk, offset);
	}
}

/* Disassembles an individual instruction
 *
 *  Params:
 *      chunk:      the chunk containing the instruction to disassemble
 *      offset:     the offset of the current instruction
 *
 *  Returns:
 *      int:        the offset value of the next instruction
 */
int disassembleInstruction(Chunk* chunk, int offset) {
	printf("%04d ", offset); // Prints the byte offset of the current instruction

	// Read a single byte at the given offset
	uint8_t instruction = chunk->code[offset];

	// Handles each instruction using utility functions to display them
	switch (instruction) {
		case OP_RETURN:
			return simpleInstruction("OP_RETURN", offset);
		default:
			printf("Unknown opcode %d\n", instruction);
			return offset + 1;
	}
}

/* Prints simple instructions
 *
 *  Params:
 *      name:       the name of the instruction (ex. OP_RETURN)
 *      offset:     the offset of the current instruction
 *
 *  Returns:
 *      int:        the offset value of the next instruction
 */
static int simpleInstruction(const char* name, int offset) {
	printf("%s\n", name);
	return offset + 1;
}