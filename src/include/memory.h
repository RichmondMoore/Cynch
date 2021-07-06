#ifndef CYNCH_MEMORY_H
#define CYNCH_MEMORY_H

// Doubles the size of the array, if the array is less than 8 bytes, grows the capacity to 8 bytes
#define GROW_CAPACITY(capacity) \
	((capacity) < 8 ? 8 : (capacity) * 2)

// Reallocates memory to a larger array
#define GROW_ARRAY(type, pointer, oldCount, newCount) \
	(type*)reallocate(pointer, sizeof(type) * (oldCount),\
			sizeof(type) * (newCount))

// Deallocates the memory of an array
#define FREE_ARRAY(type, pointer, oldCount) \
	reallocate(pointer, sizeof(type) * oldCount, 0)

void* reallocate(void* pointer, size_t oldSize, size_t newSize);

#endif //CYNCH_MEMORY_H
