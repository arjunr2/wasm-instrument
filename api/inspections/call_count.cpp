#include "inspect_common.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::string call_count_inspect(WasmModule &module) {
    json outd;
    std::list<DebugNameAssoc> *debug = module.getFnDebugNames();
    for (auto &entry : *debug) {
        outd[entry.name] = {{"calls", 0}, {"indirects", 0}, {"inst_count", 0}};
        auto &entryd = outd[entry.name];
        int idx = module.getFuncIdx(entry.func);
        if (!module.isImport(entry.func)) {
            for (auto &inst : entry.func->instructions) {
                switch (inst->getOpcode()) {
                case WASM_OP_CALL_INDIRECT:
                case WASM_OP_CALL_REF:
                    entryd["indirects"] = entryd["indirects"].get<int>() + 1;
                case WASM_OP_CALL:
                    entryd["calls"] = entryd["calls"].get<int>() + 1;
                    break;
                default: {
                };
                }
                entryd["inst_count"] = entryd["inst_count"].get<int>() + 1;
            }
        }
    }
    std::string ret = outd.dump();
    return ret;
}
