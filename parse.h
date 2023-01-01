#pragma once 

#include "common.h"
#include "ir.h"

WasmModule parse_bytecode(const byte* start, const byte* end);
