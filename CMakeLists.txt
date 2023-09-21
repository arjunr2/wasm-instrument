
cmake_minimum_required (VERSION 3.14)

project(wasm-instrument)

include(instrument_lib.cmake)

include_directories (inc .)
# --- Main executable --- #
add_executable (instrument main.cpp)
target_link_libraries (instrument instrumentlib -lm -lpthread)

# --- Test executables --- #
add_executable (test-leb tests/test-leb.cpp src/common.cpp)

add_executable (test-instlib tests/test-instlib.c)
target_link_libraries (test-instlib instrumentlib -lpthread)

add_custom_command(
  TARGET instrument
  POST_BUILD
  COMMAND ${CMAKE_COMMAND}
  ARGS -E create_symlink $<TARGET_FILE:instrument> ../instrument
)
