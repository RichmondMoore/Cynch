#include "include/common.h"
#include "include/chunk.h"
#include "include/debug.h"
#include "include/vm.h"

int main(int argc, const char* argv[]) {
	initVM();

	Chunk chunk;
	initChunk(&chunk);

	// The writeConstant() function handles both writing the OP_CONSTANT instruction to the chunk and writing the actual
	// value to it
	writeConstant(&chunk, 1.2, 123);
	writeConstant(&chunk, 3.4, 123);

	writeChunk(&chunk, OP_ADD, 123);

	writeConstant(&chunk, 5.6, 123);

	writeChunk(&chunk, OP_DIVIDE, 123);
	writeChunk(&chunk, OP_NEGATE, 123);

	writeChunk(&chunk, OP_RETURN, 123);

	disassembleChunk(&chunk, "test chunk");
	interpret(&chunk);

	freeVM();
	freeChunk(&chunk);
	return 0;
}
