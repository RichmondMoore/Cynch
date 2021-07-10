#include <stdio.h>

#include "include/memory.h"
#include "include/value.h"

/* Initializes a ValueArray
 *
 *  Params:
 *      arr:        the ValueArray to initialize
 */
void initValueArray(ValueArray* arr) {
	arr->values = NULL;
	arr->capacity = 0;
	arr->count = 0;
}

/* Writes a value to a ValueArray
 *
 *  Params:
 *      arr:        the ValueArray to write to
 *      value:      the value to be written
 *
 *  Returns:
 *      int:        the offset value of the next instruction
 */
void writeValueArray(ValueArray* arr, Value value) {
	// Allocates more memory to the array if there is no more room
	if (arr->capacity < arr->count + 1) {
		int oldCapacity = arr->capacity;
		arr->capacity = GROW_CAPACITY(oldCapacity);
		arr->values = GROW_ARRAY(Value, arr->values, oldCapacity, arr->capacity);
	}

	// Writes the new value to the array and increments the count
	arr->values[arr->count] = value;
	arr->count++;
}

/* Deallocates the memory of a ValueArray and reinitializes it
 *
 *  Params:
 *      arr:      the ValueArray to free and reinitialize
 */
void freeValueArray(ValueArray* arr) {
	FREE_ARRAY(Value, arr->values, arr->capacity);
	initValueArray(arr);
}

/* Prints a value
 *
 *  Params:
 *      value:      the value to be printed
 */
void printValue(Value value) {
	printf("%g", AS_NUMBER(value));
}