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
	chunk->lines = NULL;
	initValueArray(&chunk->constants);
}

/* Adds the data to a chunk, grows the array if necessary
 *
 * Params:
 *      chunk:      the chunk to write to
 *      byte:       the information to write to the chunk
*/
void writeChunk(Chunk* chunk, uint8_t byte, int line) {
	// If there is no more room in the array, increase its size
	if (chunk->capacity < chunk->count + 1) {
		int oldCapacity = chunk->capacity;
		chunk->capacity = GROW_CAPACITY(oldCapacity);
		chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
		chunk->lines = GROW_ARRAY(int, chunk->lines, oldCapacity, chunk->capacity);
	}

	// Then add the new data and increment the count
	chunk->code[chunk->count] = byte;
	chunk->lines[chunk->count] = line;
	chunk->count++;
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
	FREE_ARRAY(int, chunk->lines, chunk->capacity);
	freeValueArray(&chunk->constants);
	initChunk(chunk);
}