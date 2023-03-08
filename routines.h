#include <map>

#include "common.h"
#include "parse.h"
#include "ir.h"

void sample_instrument (WasmModule& module);

void loop_instrument (WasmModule &module);

std::map<FuncDecl*, uint64_t> all_funcs_weight_instrument (WasmModule &module);

uint32_t memaccess_instrument (WasmModule &module, bool dry_run = false);

void memshared_instrument (WasmModule &module, std::string path);

void memaccess_stochastic_instrument (WasmModule &module, int percent, 
    int cluster_size);

void memshared_stochastic_instrument (WasmModule &module, std::string path, 
    int percent, int cluster_size);
