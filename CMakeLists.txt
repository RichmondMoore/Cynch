cmake_minimum_required(VERSION 3.19)
project(Cynch C)

set(CMAKE_C_STANDARD 99)

add_executable(Cynch src/main.c src/include/common.h src/include/chunk.h src/chunk.c src/include/memory.h src/memory.c src/include/debug.h src/debug.c src/include/value.h src/value.c src/include/vm.h src/vm.c)