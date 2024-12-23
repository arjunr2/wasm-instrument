#include "inspect_common.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

typedef enum {
    BR_INST_TYPE = 0,
    CALL_INST_TYPE,
    CALL_INDIRECT_INST_TYPE,
    TARGET_INST_TYPE,
    RETURN_INST_TYPE,
    GENERIC_INST_TYPE
} TraceInstType;

std::string allspark_trace_blocks_inspect (WasmModule &module) {
    json outd;
    FuncDecl *start_fn = module.get_start_fn();
    if (start_fn) { 
        outd["start_funcidx"] = module.getFuncIdx(start_fn);
    }
    outd["import_func_count"] = module.getNumImportFuncs();
    outd["func_exports"] = json::object();
    for (auto &exp: module.Exports()) {
        if (exp.kind == KIND_FUNC) { 
            outd["func_exports"][exp.name] = module.getFuncIdx(exp.desc.func);
        }
    }
    for (auto &func: module.Funcs()) {
        //if (module.isImport(&func)) { continue; }
        auto func_idx = std::to_string(module.getFuncIdx(&func));
        auto inst_entries = json::array();

        auto end_pc = std::to_string(-1);

        outd[func_idx] = json::object();
        outd[func_idx]["br_blocks"] = json::object();
        // Target sites contain an index to an instruction with br_blocks
        outd[func_idx]["target_sites"] = json::object();
        outd[func_idx]["call_indirect_sites"] = json::object();
        outd[func_idx]["call_direct_sites"] = json::object();
        // To determine how to render instructions when you first enter a function
        outd[func_idx]["first_block"] = end_pc;
        outd[func_idx]["name"] = "";

        auto &br_blocks = outd[func_idx]["br_blocks"];
        auto &call_indirect_sites = outd[func_idx]["call_indirect_sites"];
        auto &call_direct_sites = outd[func_idx]["call_direct_sites"];
        auto &target_sites = outd[func_idx]["target_sites"];

        // Assign symbol for func_idx
        auto fn_debug_names = module.getFnDebugNames();
        auto dname = module.get_debug_name_from_func(&func);
        if (dname.first) {
            outd[func_idx]["name"] = dname.second;
        } else {
            TRACE("Debug name missing for Func %d!\n", module.getFuncIdx(&func));
        }

        bool first_br = true;
        auto check_first_br = [&first_br, &outd, &func_idx](std::string pc) {
            if (first_br) {
                outd[func_idx]["first_block"] = pc;
                first_br = false;
            }
        };
        auto move_br_blocks = [&br_blocks, &inst_entries, &target_sites](std::string pc) {
            br_blocks[pc] = std::move(inst_entries);
            uint32_t i = 0;
            bool first = true;
            for (auto &inst: br_blocks[pc]) {
                if ((inst["type"] == TARGET_INST_TYPE) || first) {
                    target_sites[inst["pc"]] = {
                        { "block", pc },
                        { "idx", i }
                    };
                    first = false;
                }
                i++;
            }
        };
        for (auto &inst: func.instructions) {
            std::string pc = std::to_string(inst->getOrigPc());
            uint32_t opcode = inst->getOpcode();
            inst_entries.push_back({
                { "pc", pc },
                { "inst", inst->to_string(module) },
                { "type", GENERIC_INST_TYPE }
            });
            switch(opcode) {
                case WASM_OP_BR:
                case WASM_OP_BR_IF:
                case WASM_OP_BR_TABLE: {
                    inst_entries.back()["type"] = BR_INST_TYPE;
                    move_br_blocks(pc);
                    check_first_br(pc);
                    break;
                }
                case WASM_OP_CALL_INDIRECT: {
                    inst_entries.back()["type"] = CALL_INDIRECT_INST_TYPE;
                    call_indirect_sites[pc] = nullptr;
                    break;
                }
                case WASM_OP_CALL: {
                    auto call_inst = std::static_pointer_cast<ImmFuncInst>(inst);
                    inst_entries.back()["type"] = CALL_INST_TYPE;
                    call_direct_sites[pc] = std::to_string(module.getFuncIdx(call_inst->getFunc()));
                    break;
                }
                case WASM_OP_RETURN: {
                    inst_entries.back()["type"] = RETURN_INST_TYPE;
                    break;
                }
                case WASM_OP_END:
                case WASM_OP_LOOP: {
                    inst_entries.back()["type"] = TARGET_INST_TYPE;
                    break;
                }
                default:
                    break;
            }
        }
        move_br_blocks(end_pc);
        target_sites[end_pc] = 0;
    }

    return outd.dump();
}