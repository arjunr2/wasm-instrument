#pragma once 

#include "common.h"
#include "ir.h"

wasm_module_t parse_bytecode(const byte* start, const byte* end);
