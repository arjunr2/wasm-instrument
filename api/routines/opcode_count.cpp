#include "routine_common.h"

#define PUSH_INST(inv) addinst.push_back(InstBasePtr(new inv));

static std::map<uint16_t, GlobalDecl *> found_ops;
static std::map<uint16_t, uint64_t> run_counts;

static void insert_opcode_instrumentation(WasmModule &module, InstItr &institr,
                                          FuncDecl *func) {
    static GlobalDecl global = {.type = WASM_TYPE_I64,
                                .is_mutable = true,
                                .init_expr_bytes = INIT_EXPR(I64_CONST, 0)};

    InstList addinst;
    TRACE("Inserting instrumentation here\n");
    for (auto it = run_counts.begin(); it != run_counts.end(); ++it) {
        uint16_t opcode = it->first;
        uint64_t count = it->second;
        GlobalDecl *gref = NULL;
        if (!(gref = found_ops[opcode])) {
            gref = found_ops[opcode] = module.add_global(
                global,
                (std::string("__oc_") + std::to_string(opcode)).c_str());
        }
        PUSH_INST(GlobalGetInst(gref));
        PUSH_INST(I64ConstInst(count));
        PUSH_INST(I64AddInst());
        PUSH_INST(GlobalSetInst(gref));
    }
    // Insert into codestream
    InstList &insts = func->instructions;
    insts.insert(institr, addinst.begin(), addinst.end());
    // Reset run counts
    run_counts.clear();
}

static void scope_opcode_instrument(WasmModule &module, ScopeBlock *scope,
                                    FuncDecl *func) {
    auto next_outer_subscope = scope->outer_subscopes.begin();
    auto institr = scope->start;
    for (institr = scope->start; institr != scope->end; ++institr) {
        ScopeBlock *outer_subscope = *next_outer_subscope;
        // For scope instructions, recursive invocation if found
        if ((next_outer_subscope != scope->outer_subscopes.end()) &&
            (institr == outer_subscope->start)) {
            if (outer_subscope->get_scope_type() == UNKNOWN) {
                ERR("Invalid scope type\n");
            }
            scope_opcode_instrument(module, outer_subscope, func);
            // Skip over subscope
            institr = outer_subscope->end;
        } else {
            InstBasePtr instptr = *institr;
            // Skip over scope inst (block, loop, if/else)
            if (isScopeInst(instptr)) {
                continue;
            }
            TRACE("O: %s\n", opcode_table[instptr->getOpcode()].mnemonic);
            // For all other instructions, add to profile
            uint16_t opcode = instptr->getOpcode();
            run_counts[opcode]++;
            // For branches and return, instrument
            if ((instptr->isImmType(IMM_LABEL) ||
                 instptr->isImmType(IMM_LABELS)) ||
                instptr->is(WASM_OP_RETURN)) {
                insert_opcode_instrumentation(module, institr, func);
            }
        }
    }
    // Instrument at end of scope
    insert_opcode_instrumentation(module, institr, func);
}

void opcode_count_instrument(WasmModule &module) {
    for (auto &func : module.Funcs()) {
        ScopeList scope_list = module.gen_scopes_from_instructions(&func);
        if (!scope_list.empty())
            scope_opcode_instrument(module, &scope_list.front(), &func);
    }
}
