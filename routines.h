#include <map>

#include "common.h"
#include "parse.h"
#include "ir.h"

void sample_instrument (WasmModule& module);

void loop_instrument (WasmModule &module);

std::map<FuncDecl*, uint64_t> all_funcs_weight_instrument (WasmModule &module);

void memaccess_instrument (WasmModule &module, const std::string& path);

std::vector<WasmModule> memaccess_stochastic_instrument (WasmModule &module, 
    int percent, int cluster_size, const std::string& path);

std::vector<WasmModule> memaccess_balanced_instrument (WasmModule &module, 
    int cluster_size, const std::string& path);
