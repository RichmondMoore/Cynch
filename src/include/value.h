#ifndef CYNCH_VALUE_H
#define CYNCH_VALUE_H

#include "common.h"

typedef enum {
	VAL_BOOL,
	VAL_NIL,
	VAL_NUMBER
} ValueType;

typedef struct {
	ValueType type;
	union {
		bool boolean;
		double number;
	} as;
} Value;

// Checks a value's type
#define IS_BOOL(value)          ((value.type) == VAL_BOOL)
#define IS_NIL(value)           ((value.type) == VAL_NIL)
#define IS_NUMBER(value)        ((value.type) == VAL_NUMBER)

// Given a value, returns the corresponding C value
#define AS_BOOL(value)          ((value).as.boolean)
#define AS_NUMBER(value)        ((value).as.number)

// Produces a value of a given type
#define BOOL_VAL(value)         ((Value){VAL_BOOL, {.boolean = value}})
#define NIL_VAL(value)          ((Value){VAL_NIL, {.number = 0}})
#define NUMBER_VAL(value)       ((Value){VAL_NUMBER, {.number = value}})

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
