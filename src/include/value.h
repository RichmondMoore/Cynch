#ifndef CYNCH_VALUE_H
#define CYNCH_VALUE_H

#include "common.h"

typedef double Value;

typedef struct {
	int capacity;
	int count;
	Value* values;
} ValueArray;

void initValueArray(ValueArray* arr);
void writeValueArray(ValueArray* arr, Value value);
void freeValueArray(ValueArray* arr);
void printValue(Value value);

#endif //CYNCH_VALUE_H
