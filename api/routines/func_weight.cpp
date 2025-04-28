#include "routine_common.h"

#define MAX(A, B) ({ ((A > B) ? (A) : (B)); })

uint64_t func_weight_instrument(WasmModule &module, FuncDecl *func,
                                std::map<FuncDecl *, uint64_t> &func_weights);

static uint64_t get_weight(InstBasePtr instptr, WasmModule &module,
                           FuncDecl *func,
                           std::map<FuncDecl *, uint64_t> &func_weights) {
    switch (instptr->getImmType()) {
    // Memory accesses
    case IMM_GLOBAL:
    case IMM_LOCAL:
    case IMM_TABLE:
    case IMM_MEMARG: {
        return 3;
    }
    // Label
    case IMM_LABEL:
    case IMM_LABELS: {
        return 2;
    }
    // Indirect call
    case IMM_SIG_TABLE: {
        return 1000;
    }
    // Direct call: Scan next function
    case IMM_FUNC: {
        std::shared_ptr<ImmFuncInst> call_inst =
            static_pointer_cast<ImmFuncInst>(instptr);
        FuncDecl *call_func = call_inst->getFunc();
        return func_weight_instrument(module, call_func, func_weights);
    }
    // Consts
    case IMM_I32:
    case IMM_I64:
    case IMM_F32:
    case IMM_F64: {
        return 0;
    }
    default:
        return 1;
    }
}

static uint64_t
scope_weight_instrument(WasmModule &module, ScopeBlock *scope, FuncDecl *func,
                        std::map<FuncDecl *, uint64_t> &func_weights) {
    uint64_t weight = 0;
    auto next_outer_subscope = scope->outer_subscopes.begin();
    for (auto institr = scope->start; institr != scope->end; ++institr) {
        InstBasePtr instptr = *institr;
        ScopeBlock *outer_subscope = *next_outer_subscope;
        TRACE("O: %s\n", opcode_table[instptr->getOpcode()].mnemonic);
        // For scope instructions, get scope weight and advance past scope if
        // found
        if ((next_outer_subscope != scope->outer_subscopes.end()) &&
            (institr == outer_subscope->start)) {
            switch (outer_subscope->get_scope_type()) {
            case BLOCK:
            case LOOP:
            case IF: {
                uint64_t ifweight = scope_weight_instrument(
                    module, outer_subscope, func, func_weights);
                institr = outer_subscope->end;
                std::advance(next_outer_subscope, 1);
                weight += ifweight;
                break;
            }
            // Must never encounter ELSE; handled within IFWELSE
            case IFWELSE: {
                uint64_t ifweight = scope_weight_instrument(
                    module, outer_subscope, func, func_weights);
                outer_subscope = *std::next(next_outer_subscope);
                uint64_t elseweight = scope_weight_instrument(
                    module, outer_subscope, func, func_weights);
                std::advance(next_outer_subscope, 2);
                weight += MAX(ifweight, elseweight);
                break;
            }
            case UNKNOWN: {
                ERR("Invalid scope type\n");
                break;
            }
            }
        }
        // For return, just end
        else if (instptr->is(WASM_OP_RETURN)) {
            return weight;
        }
        // Other instructions, weight=1
        else {
            weight += get_weight(instptr, module, func, func_weights);
        }
    }
    TRACE("Scope weight: %lu\n", weight);
    return weight;
}

// Returns weight of the function
uint64_t func_weight_instrument(WasmModule &module, FuncDecl *func,
                                std::map<FuncDecl *, uint64_t> &func_weights) {
    if (module.isImport(func)) {
        func_weights[func] = 10000;
    }

    else if (!func_weights.contains(func)) {
        ScopeList scope_list = module.gen_scopes_from_instructions(func);
        uint64_t func_weight = scope_weight_instrument(
            module, &scope_list.front(), func, func_weights);
        TRACE("<====>\n");
        func_weights[func] = func_weight;
    }
    return func_weights[func];
}

std::map<FuncDecl *, uint64_t> all_funcs_weight_instrument(WasmModule &module) {
    // Stores intermediate function weights to avoid recomputation
    std::map<FuncDecl *, uint64_t> func_weights;
    for (auto &func : module.Funcs()) {
        func_weight_instrument(module, &func, func_weights);
    }
    return func_weights;
}
