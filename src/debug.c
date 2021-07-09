#include <stdio.h>

#include "include/debug.h"
#include "include/value.h"

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

	if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
		printf("   | "); // Instructions from the same line are shown using this print statement
	} else {
		printf("%4d ", chunk->lines[offset]);
	}

	// Read a single byte at the given offset
	uint8_t instruction = chunk->code[offset];

	// Handles each instruction using utility functions to display them
	switch (instruction) {
		case OP_CONSTANT:
			return constantInstruction("OP_CONSTANT", chunk, offset);
		case OP_CONSTANT_LONG:
			return longConstantInstruction("OP_CONSTANT_LONG", chunk, offset);
		case OP_ADD:
			return simpleInstruction("OP_ADD", offset);
		case OP_SUBTRACT:
			return simpleInstruction("OP_SUBTRACT", offset);
		case OP_MULTIPLY:
			return simpleInstruction("OP_MULTIPLY", offset);
		case OP_DIVIDE:
			return simpleInstruction("OP_DIVIDE", offset);
		case OP_NEGATE:
			return simpleInstruction("OP_NEGATE", offset);
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

/* Prints the constant value at the specified offset
 *
 *  Params:
 *      name:       the name of the chunk
 *      chunk:      the chunk containing the value
 *      offset:     the constant index (byte after the opcode name)
 */
static int constantInstruction(const char* name, Chunk* chunk, int offset) {
	uint8_t constant = chunk->code[offset + 1];
	printf("%-16s %4d '", name, constant);
	printValue(chunk->constants.values[constant]);
	printf("'\n");

	// Two bytes: one for the name, one for the operand
	return offset + 2;
}

static int longConstantInstruction(const char* name, Chunk* chunk, int offset) {
	uint32_t constant = chunk->code[offset + 1] |
						(chunk->code[offset + 2] << 8) |
						(chunk->code[offset + 3] << 16);
	printf("%-16s %4d '", name, constant);
	printValue(chunk->constants.values[constant]);
	printf("'\n");

	// Four bytes: one for the name, three for the operand
	return offset + 4;
}