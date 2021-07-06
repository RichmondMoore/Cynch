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
}

/* Adds the data to a chunk, grows the array if necessary
 *
 * Params:
 *      chunk:      the chunk to write to
 *      byte:       the information to write to the chunk
*/
void writeChunk(Chunk* chunk, uint8_t byte) {
	// If there is no more room in the array, increase its size
	if (chunk->capacity < chunk->count + 1) {
		int oldCapacity = chunk->capacity;
		chunk->capacity = GROW_CAPACITY(oldCapacity);
		chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
	}

	// Then add the new data and increment the count
	chunk->code[chunk->count] = byte;
	chunk->count++;
}

/* Deallocates the memory of a chunk and reinitializes it
 *
 *  Params:
 *      chunk:      the chunk to free and reinitialize
 */
void freeChunk(Chunk* chunk) {
	FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
	initChunk(chunk);
}