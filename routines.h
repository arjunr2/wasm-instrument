#include <map>

#include "common.h"
#include "parse.h"
#include "ir.h"

void sample_instrument (WasmModule& module);

void loop_instrument (WasmModule &module);

std::map<FuncDecl*, uint64_t> all_funcs_weight_instrument (WasmModule &module);
