#include <stdlib.h>

#include "include/chunk.h"
#include "include/memory.h"

/* Initializes a chunk
 *
 *  Params:
 *      chunk:      the chunk to initialize
*/
void initChunk(Chunk* chunk) {
	chunk->count = 0;
	chunk->capacity = 0;
	chunk->code = NULL;
	chunk->lineCount = 0;
	chunk->lineCapacity = 0;
	chunk->lines = NULL;
	initValueArray(&chunk->constants);
}

/* Adds the data to a chunk, grows the arrays if necessary
 *
 * Params:
 *      chunk:      the chunk to write to
 *      byte:       the information to write to the chunk
 *      line:       the source line of the chunk
*/
void writeChunk(Chunk* chunk, uint8_t byte, int line) {
	// If there is no more room in the array, increase its size
	if (chunk->capacity < chunk->count + 1) {
		int oldCapacity = chunk->capacity;
		chunk->capacity = GROW_CAPACITY(oldCapacity);
		chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
	}

	// Then add the new data and increment the count
	chunk->code[chunk->count] = byte;
	chunk->count++;

	// Checks if chunk is still on the same line
	if (chunk->lineCount > 0 && chunk->lines[chunk->lineCount - 1].line == line) {
		return;
	}

	// Otherwise, a LineStart needs to be added
	// Checks if there is room in the array, grows if necessary
	if (chunk->lineCapacity < chunk->lineCount + 1) {
		int oldCapacity = chunk->lineCapacity;
		chunk->lineCapacity = GROW_CAPACITY(oldCapacity);
		chunk->lines = GROW_ARRAY(LineStart, chunk->lines, oldCapacity, chunk->lineCapacity);
	}

	// Adds the new LineStart
	LineStart* lineStart = &chunk->lines[chunk->lineCount++];
	lineStart->offset = chunk->count - 1;
	lineStart->line = line;
}

/* Adds a value to a chunk's list of constants
 *
 *  Params:
 *      chunk:      the chunk to that will contain the value
 *      value:      the constant to be included in the chunk
 *
 *  Returns:
 *      The index of the added value
 */
int addConstant(Chunk* chunk, Value value) {
	writeValueArray(&chunk->constants, value);
	return chunk->constants.count - 1;
}

/* Adds the value to the chunk, then stores that information in the code, using three bytes (24 bits for a long)
 *
 * Params:
 *      chunk:      the chunk to write to
 *      byte:       the information to write to the chunk
*/
void writeConstant(Chunk* chunk, Value value, int line) {
	int index = addConstant(chunk, value);

	if (index < 256) {
		writeChunk(chunk, OP_CONSTANT, line);
		writeChunk(chunk, (uint8_t)index, line);
	} else {
		writeChunk(chunk, OP_CONSTANT_LONG, line);
		writeChunk(chunk, (uint8_t)(index & 0xff), line);
		writeChunk(chunk, (uint8_t)((index >> 8) & 0xff), line);
		writeChunk(chunk, ((uint8_t)(index >> 16) & 0xff), line);
	}
}

/* Deallocates the memory of a chunk and reinitializes it
 *
 *  Params:
 *      chunk:      the chunk to free and reinitialize
 */
void freeChunk(Chunk* chunk) {
	FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
	FREE_ARRAY(LineStart, chunk->lines, chunk->lineCapacity);
	freeValueArray(&chunk->constants);
	initChunk(chunk);
}

/* Finds the current line given the chunk and instruction using a binary search.
 *
 *  Params:
 *      chunk:          the chunk to search
 *      instruction:    the instruction to find within the chunk
 *
 *  Returns:
 *      The line number of the specified instruction within the chunk.
 */
int getLine(Chunk* chunk, int instruction) {
	int start = 0;
	int end = chunk->lineCount - 1;

	for (;;) {
		int mid = (start + end) / 2;
		LineStart* line = &chunk->lines[mid];
		if (instruction < line->offset) {
			end = mid - 1;
		} else if (mid == chunk->lineCount - 1 || instruction < chunk->lines[mid + 1].offset) {
			return line->line;
		} else {
			start = mid + 1;
		}
	}
}