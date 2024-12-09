#include "routine_common.h"
#include <cassert>

// Record the following information for every "scope"
// 1. Scope ID (Statically determined)
// 2. Entire Memory State After Execution of the Basic Block
// 3. I/O calls to the program
// 4. Distribution of Bytecodes for the Basic Block
// (optional) 5. Instruction + memory address, size mapping for 
//   each memory instruction in basic block


// Identifying Scopes
// 1. Anything that modifies control flow (branches, returns)
// 2. Scope starts -- loop, block, if/else
// 3. Scope end and end of function

// Assumptions: Wasm code only uses the bulk-memory extension 
// (no threads, exceptions, simd)

#define INST(inv) InstBasePtr(new inv)

#define LARGEST_OPCODE (1 << 12)
#define LARGEST_BASE_OPCODE (1 << 8)
#define OPCOUNT_SIZE (1 << 3)

struct TraceInstrumentData {
    // Number of bytes for storing control flow
    GlobalDecl* ctrlflow_bytes;
    // Vector of control flow
    GlobalDecl* ctrlflow_vector;
    FuncDecl* main_fn;
    FuncDecl* curr_fn;
    uint32_t tmp_i32_locals[2];
    FuncDecl* tracedump_fn;

    TraceInstrumentData(WasmModule &module, FuncDecl* main_fn, uint32_t instmem_start) {
        GlobalDecl ctrlflow_brsite_gdecl = {
            .type = WASM_TYPE_I32,
            .is_mutable = true,
            .init_expr_bytes = INIT_EXPR(I32_CONST, 0),
        };

        GlobalDecl ctrlflow_bytes_gdecl = {
            .type = WASM_TYPE_I32,
            .is_mutable = true,
            .init_expr_bytes = INIT_EXPR(I32_CONST, 0),
        };

        GlobalDecl ctrlflow_vector_gdecl = {
            .type = WASM_TYPE_I32,
            .is_mutable = true,
            .init_expr_bytes = INIT_EXPR(I32_CONST, instmem_start),
        };

        this->ctrlflow_bytes = module.add_global(ctrlflow_bytes_gdecl, "ctrlflow_bytes");
        this->ctrlflow_vector = module.add_global(ctrlflow_vector_gdecl, "ctrlflow_vector");
        this->main_fn = main_fn;
        
        // Parameters for tracedump function are as follows:
        // 1: Address of control flow map buffer
        // 2: Size of buffer in bytes
        SigDecl s = { .params = { WASM_TYPE_I32, WASM_TYPE_I32 }, 
            .results = { } };
        ImportInfo iminfo = { .mod_name = "allspark-trace", .member_name = "prog_tracedump" };
        ImportDecl* import_decl = module.add_import(iminfo, s); 
        this->tracedump_fn = import_decl->desc.func;
    }

    void set_current_fn(FuncDecl* func) {
        this->tmp_i32_locals[0] = func->add_local(WASM_TYPE_I32);
        this->tmp_i32_locals[1] = func->add_local(WASM_TYPE_I32);
        this->curr_fn = func;
    }
};

void local_cfscope_handler(WasmModule &module, ScopeBlock &scope, 
        TraceInstrumentData &trace_data) {

    static std::vector<ScopeBlock*> cfscope_stack;

    cfscope_stack.push_back(&scope);    
    // Depth-first recursion of subscopes
    for (auto &subscope : scope.outer_subscopes) {
        //TRACE("[PC: %u] Recurse subscope\n", (*(subscope->start))->getOrigPc());
        local_cfscope_handler(module, *subscope, trace_data);
    }

    TRACE("Processing cfscope [s:%u, e:%u]\n",
        (*(scope.start))->getOrigPc(), (*(scope.end))->getOrigPc());

    switch (scope.get_scope_type()) {
        case IF:
        case IFWELSE:
        case ELSE: {
            ERR("Cannot handle IF/ELSE opcodes yet; terminate\n");
            throw std::runtime_error("IF/ELSE opcode not supported");
            break;
        }
        default: {}
    }

    auto next_outer_subscope = scope.outer_subscopes.begin();

    for (auto institr = scope.start; institr != scope.end; ++institr) {
        InstBasePtr instptr = *institr;
        uint16_t opcode = instptr->getOpcode();
        uint16_t pc = instptr->getOrigPc();

        InstBuilder builder = {};

        auto get_br_target_pc = [] (uint32_t depth) -> uint32_t {
            uint32_t stack_idx = cfscope_stack.size() - 1 - depth;
            ScopeBlock &target = *cfscope_stack[stack_idx];
            // Target on stack
            if (target.get_scope_type() == LOOP) {
                return (*(target.start))->getOrigPc();
            } else {
                return (*(target.end))->getOrigPc();
            }
        };

        // Instructions to record ctrlflow path into vector in memory
        auto record_ctrlflow = [&module, &trace_data, &builder, &instptr]() {
            uint64_t brpc_shift = ((uint64_t)instptr->getOrigPc() << 32);
            builder.push ({
                // Control flow target
                INST (LocalSetInst(trace_data.tmp_i32_locals[0])),
                // Base address to write target entry
                INST (GlobalGetInst(trace_data.ctrlflow_vector)),
                INST (GlobalGetInst(trace_data.ctrlflow_bytes)),
                INST (I32AddInst()),
                // Temp for the base address
                INST (LocalTeeInst(trace_data.tmp_i32_locals[1])),
                // Pack br_pc, target and write
                INST (I64ConstInst(brpc_shift)),
                INST (LocalGetInst(trace_data.tmp_i32_locals[0])),
                INST (I64ExtendI32UInst()),
                INST (I64OrInst()),
                INST (I64StoreInst(3, 0, module.getMemory(0))),
                // Increment ctrlflow_bytes
                INST (GlobalGetInst(trace_data.ctrlflow_bytes)),
                INST (I32ConstInst(8)),
                INST (I32AddInst()),
                INST (GlobalSetInst(trace_data.ctrlflow_bytes)),
            });
        };

        auto br_insert = [&module, &trace_data, &pc, &institr, &instptr, 
                &builder, &get_br_target_pc, &record_ctrlflow]() {
            std::shared_ptr<ImmLabelInst> br_inst = std::static_pointer_cast<ImmLabelInst>(instptr);
            uint32_t target_pc = get_br_target_pc(br_inst->getLabel());
            // Target on stack
            builder.push_inst(I32ConstInst(target_pc));
            record_ctrlflow();
            builder.splice_into(trace_data.curr_fn->instructions, institr);
        };
        auto br_if_insert = [&module, &trace_data, &pc, &institr, &instptr, 
                &builder, &get_br_target_pc, &record_ctrlflow]() {
            std::shared_ptr<ImmLabelInst> br_if_inst = std::static_pointer_cast<ImmLabelInst>(instptr);
            uint32_t succ_pc = get_br_target_pc(br_if_inst->getLabel());
            uint32_t fail_pc = (*std::next(institr))->getOrigPc();
            builder.push ({
                // Save br_cond
                INST (LocalTeeInst(trace_data.tmp_i32_locals[0])),
                INST (I32ConstInst(succ_pc)),
                INST (I32ConstInst(fail_pc)),
                INST (LocalGetInst(trace_data.tmp_i32_locals[0])),
                INST (SelectInst())
            });
            record_ctrlflow();
            builder.splice_into(trace_data.curr_fn->instructions, institr);
        };
        auto br_table_insert = [&module, &trace_data, &pc, &institr, &instptr, 
                &builder, &get_br_target_pc, &record_ctrlflow]() {
            std::shared_ptr<ImmLabelsInst> br_table_inst = std::static_pointer_cast<ImmLabelsInst>(instptr);
            auto labels = br_table_inst->getLabels();
            // Get the number of arms
            uint32_t max_idx = *std::max_element(labels.begin(), labels.end());
            max_idx = std::max(max_idx, br_table_inst->getDefLabel());
            // 
            SigDecl i32_ret_sigdecl = { .params = { }, .results = { WASM_TYPE_I32 } };   
            SigDecl *block_sig = module.add_sig(i32_ret_sigdecl, false);
            uint32_t block_sig_idx = module.getSigIdx(block_sig);

            // Store dynamic branch site in local
            builder.push_inst (LocalTeeInst(trace_data.tmp_i32_locals[0]));
            for (int i = 0; i < max_idx + 1; i++) {
                builder.push_inst(BlockInst(block_sig_idx));
            }
            builder.push ({
                INST (BlockInst(block_sig_idx)),
                INST (I32ConstInst(0)),
                INST (LocalGetInst(trace_data.tmp_i32_locals[0])),
                INST (ImmLabelsInst(*br_table_inst)),
                INST (EndInst())
            });
            for (int i = 0; i < max_idx + 1; i++) {
                uint32_t target_pc = get_br_target_pc(i);
                // Target on stack
                builder.push ({
                    INST (I32ConstInst(target_pc)),
                    INST (BrInst(max_idx - i)),
                    INST (EndInst())
                });
            }
            record_ctrlflow();
            builder.splice_into(trace_data.curr_fn->instructions, institr);
        };

        TRACE("[PC: %u] [OP: %s]\n", pc, opcode_table[opcode].mnemonic);
        if ((next_outer_subscope != scope.outer_subscopes.end()) && (institr == (*next_outer_subscope)->start)) {
            TRACE("Found start of a subscope; Skip\n");
            institr = (*next_outer_subscope)->end;
            std::advance(next_outer_subscope, 1);
            continue;
        }

        // Monitor before instructions that modify control flow
        switch (opcode) {
            case WASM_OP_BR: { br_insert(); break; }
            case WASM_OP_BR_IF: { br_if_insert(); break; }
            case WASM_OP_BR_TABLE: { br_table_insert(); break; }
            default: {}
        }
    }

    cfscope_stack.pop_back();
}

// Main Allspark Trace Instrumenter
void allspark_trace_instrument (WasmModule &module, std::string entry_export) {
    printf("Allspark Trace Instrumentation\n");
    printf("Entry Export: %s\n", entry_export.c_str());

    auto exp = module.find_export(entry_export);
    if (!exp || (exp->kind != KIND_FUNC)) {
        ERR("Entry func export \'%s\' not found\n", entry_export.c_str());
        return;
    }

    // Get the main function
    FuncDecl* main_fn = exp->desc.func;

    // Increase memory a lot to store the vectors
    uint32_t old_page_count = add_pages(module.getMemory(0), 16384);

    TraceInstrumentData trace_data(module, main_fn, old_page_count * WASM_PAGE_SIZE);

    // Generate scopes for the main function
    for (auto &func: module.Funcs()) {
        if (module.isImport(&func)) { continue; }
        ScopeList main_scopes = module.gen_scopes_from_instructions(&func);
        ScopeBlock main_scope = main_scopes.front();

        trace_data.set_current_fn(&func);
        local_cfscope_handler(module, main_scope, trace_data); 
    }

    auto insert_tracedump = [&module, &trace_data](auto institr) {
        InstBuilder builder = {};
        builder.push ({
            INST (GlobalGetInst(trace_data.ctrlflow_vector)),
            INST (GlobalGetInst(trace_data.ctrlflow_bytes)),
            INST (CallInst(trace_data.tracedump_fn)),
        });
        builder.splice_into(trace_data.curr_fn->instructions, institr);
    };

    trace_data.set_current_fn(trace_data.main_fn);
    // Dump vector trace at the end of the main function and returns
    for (auto institr = main_fn->instructions.begin(); institr != main_fn->instructions.end(); ++institr) {
        InstBasePtr instptr = *institr;
        uint16_t opcode = instptr->getOpcode();
        if (opcode == WASM_OP_RETURN) {
            insert_tracedump(institr);
        }
    }
    insert_tracedump(std::prev(main_fn->instructions.end()));

}