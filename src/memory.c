#include <stdlib.h>

#include "include/memory.h"

// Handles all memory management: allocating, freeing, and changing allocation sizes
void* reallocate(void* pointer, size_t oldSize, size_t newSize) {

	// A newSize of 0 indicates a free allocation
	if (newSize == 0) {
		free(pointer);
		return NULL;
	}

	// Other cases are handled by realloc()
	void* result = realloc(pointer, newSize);
	if (result == NULL) exit(1); // If realloc() fails, exit the program
	return result;
}