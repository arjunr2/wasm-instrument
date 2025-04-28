#include <map>

#include "common.h"
#include "ir.h"
#include "parse.h"

const std::string start_export_str = "_start";

void sample_instrument(WasmModule &module);

void loop_instrument(WasmModule &module);

void opcode_count_instrument(WasmModule &module);

std::map<FuncDecl *, uint64_t> all_funcs_weight_instrument(WasmModule &module);

void memaccess_instrument(WasmModule &module, const std::string &path);

std::vector<WasmModule> memaccess_stochastic_instrument(
    WasmModule &module, int percent, int cluster_size, const std::string &path,
    void (*encode_callback)(WasmModule &, int) = NULL);

std::vector<WasmModule> memaccess_balanced_instrument(WasmModule &module,
                                                      int cluster_size,
                                                      const std::string &path);

void func_entry_instrument(WasmModule &module);

void r3_record_instrument(WasmModule &module);

void r3_replay_instrument(WasmModule &module, void *replay_ops,
                          uint32_t num_ops, int64_t flags);

void allspark_trace_instrument(WasmModule &module,
                               std::string entry_export = start_export_str);

void snapshot_instrument(WasmModule &module);
