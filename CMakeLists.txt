
cmake_minimum_required (VERSION 3.14)

project(wasm-instrument)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

file (GLOB_RECURSE INSTRUMENT_LIB_SRCS src/*.c src/*.cpp api/*.cpp)

add_library (instrumentlib ${INSTRUMENT_LIB_SRCS})
target_include_directories (instrumentlib PRIVATE inc .)

add_executable (instrument main.cpp)
target_include_directories (instrument PRIVATE inc .)

target_link_libraries (instrument instrumentlib -lm -lpthread)