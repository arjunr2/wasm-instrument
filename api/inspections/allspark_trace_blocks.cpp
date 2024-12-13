#include "inspect_common.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::string allspark_trace_blocks_inspect (WasmModule &module) {
    json outd;
    for (auto &func: module.Funcs()) {
        if (module.isImport(&func)) { continue; }
        auto func_idx = std::to_string(module.getFuncIdx(&func));
        auto inst_entries = json::array();
        /* All possible locations where */
        //auto target_entries = std::vector<uint32_t>;
        outd[func_idx] = json::object();
        for (auto &inst: func.instructions) {
            uint32_t pc = inst->getOrigPc();
            uint32_t opcode = inst->getOpcode();
            inst_entries.push_back({
                { "pc", pc },
                { "inst", inst->to_string(module) }
            });
            switch(opcode) {
                case WASM_OP_BR:
                case WASM_OP_BR_IF:
                case WASM_OP_BR_TABLE:
                    outd[func_idx][std::to_string(pc)] = std::move(inst_entries);
                    break;
                //case WASM_OP_END:
                //case WASM_OP_LOOP
                default:
                    break;
            }
        }
        auto end_pc = std::to_string((uint32_t)-1);
        outd[func_idx][end_pc] = std::move(inst_entries);
    }

    return outd.dump();
}