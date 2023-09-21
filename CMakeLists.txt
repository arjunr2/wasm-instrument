
cmake_minimum_required (VERSION 3.14)

project(wasm-instrument)

include(instrument_lib.cmake)

add_executable (instrument main.cpp)
target_include_directories (instrument PRIVATE inc .)

target_link_libraries (instrument instrumentlib -lm -lpthread)