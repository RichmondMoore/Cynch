#include <stdarg.h>
#include <stdio.h>

#include "include/common.h"
#include "include/compiler.h"
#include "include/debug.h"
#include "include/memory.h"
#include "include/vm.h"

VM vm;

static void resetStack() {
	vm.stackCount = 0;
}

static void runtimeError(const char* format, ...) {
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fputs("\n", stderr);

	size_t instruction = vm.ip - vm.chunk->code - 1;
	int line = getLine(vm.chunk, instruction);
	fprintf(stderr, "[line %d] in script\n", line);
	resetStack();
}

void initVM() {
	vm.stack = NULL;
	vm.stackCapacity = 0;
	resetStack();
}

void freeVM() {

}

void push(Value value) {
	if (vm.stackCapacity < vm.stackCount + 1) {
		int oldCapacity = vm.stackCapacity;
		vm.stackCapacity = GROW_CAPACITY(oldCapacity);
		vm.stack = GROW_ARRAY(Value, vm.stack, oldCapacity, vm.stackCapacity);
	}

	vm.stack[vm.stackCount] = value;
	vm.stackCount++;
}

Value pop() {
	vm.stackCount--;
	return vm.stack[vm.stackCount];
}

static Value peek(int distance) {
	return vm.stack[vm.stackCount - distance - 1];
}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(valueType, op) \
    do { \
      if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
        runtimeError("Operands must be numbers."); \
        return INTERPRET_RUNTIME_ERROR; \
      } \
      double b = AS_NUMBER(pop()); \
      double a = AS_NUMBER(pop()); \
      push(valueType(a op b)); \
    } while (false)

	for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
		printf("          ");
		for (int index = 0; index < vm.stackCount; index++) {
			printf("[ ");
			printValue(vm.stack[index]);
			printf(" ]");
		}
		printf("\n");
		disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
		uint8_t instruction;
		switch (instruction = READ_BYTE()) {
			case OP_CONSTANT: {
				Value constant = READ_CONSTANT();
				push(constant);
				break;
			}
			case OP_ADD:      BINARY_OP(NUMBER_VAL, +); break;
			case OP_SUBTRACT: BINARY_OP(NUMBER_VAL, -); break;
			case OP_MULTIPLY: BINARY_OP(NUMBER_VAL, *); break;
			case OP_DIVIDE:   BINARY_OP(NUMBER_VAL, /); break;
			case OP_NEGATE:
				if (!IS_NUMBER(peek(0))) {
					runtimeError("Operand must be a number.");
					return INTERPRET_RUNTIME_ERROR;
				}
				push(NUMBER_VAL(-AS_NUMBER(pop())));
				break;
			case OP_RETURN: {
				printf("\n");
				printValue(pop());
				printf("\n");
				return INTERPRET_OK;
			}
		}
	}

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

/* Interprets source code from a file:
 *      Compiles the source code and fills a chunk with the corresponding bytecode. If there is a compilation error,
 *      compile() returns false and the chunk is discarded. Otherwise, the chunk is sent to the VM to be executed.
 *
 *  Returns:
 *      Returns if there was an error and if it is from compilation or runtime.
 */
InterpretResult interpret(const char* source) {
	Chunk chunk;
	initChunk(&chunk);

	if (!compile(source, &chunk)) {
		freeChunk(&chunk);
		return INTERPRET_COMPILE_ERROR;
	}

	vm.chunk = &chunk;
	vm.ip = vm.chunk->code;

	InterpretResult result = run();

	freeChunk(&chunk);
	return result;
}