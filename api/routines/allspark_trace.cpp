#include "routine_common.h"
#include <cassert>
#include <cmath>

// Record the following information for every branch
// Function index, Source PC, Target PC

#define LARGEST_OPCODE (1 << 12)
#define LARGEST_BASE_OPCODE (1 << 8)
#define OPCOUNT_SIZE (1 << 3)

// Use globals (as opposed to memory) to store tracing metadata
#define USE_GLOBALS_TRACE 0
// Use atomics to update memory (when USE_GLOBALS_TRACE == 0)
#define USE_ATOMICS 0

struct TraceInstrumentData {
#if USE_GLOBALS_TRACE != 0
    // Number of bytes for storing control flow
    GlobalDecl *ctrlflow_bytes;
    // Vector of control flow
    GlobalDecl *ctrlflow_vector;
#endif
    // Memory addresses for these: both u32 (4 bytes) if USE_GLOBALS_TRACE == 0
    uint32_t ctrlflow_bytes_addr;
    uint32_t ctrlflow_vector_start;

    FuncDecl *main_fn;
    FuncDecl *curr_fn;

    LocalAllocator lc_allocator;
    FuncDecl *tracedump_fn;

    uint32_t pc_bits;
    uint32_t fnidx_bits;

    TraceInstrumentData(WasmModule &module, FuncDecl *main_fn,
                        uint32_t instmem_start, uint32_t pc_bits,
                        uint32_t fnidx_bits)
        : lc_allocator(NULL, {}) {

        this->ctrlflow_bytes_addr = instmem_start;
        this->ctrlflow_vector_start = instmem_start + 8;

#if USE_GLOBALS_TRACE != 0
        GlobalDecl ctrlflow_bytes_gdecl = {
            .type = WASM_TYPE_I32,
            .is_mutable = true,
            .init_expr_bytes = INIT_EXPR(I32_CONST, 0),
        };

        GlobalDecl ctrlflow_vector_gdecl = {
            .type = WASM_TYPE_I32,
            .is_mutable = true,
            .init_expr_bytes =
                INIT_EXPR(I32_CONST, this->ctrlflow_vector_start),
        };

        this->ctrlflow_bytes =
            module.add_global(ctrlflow_bytes_gdecl, "ctrlflow_bytes");
        this->ctrlflow_vector =
            module.add_global(ctrlflow_vector_gdecl, "ctrlflow_vector");
#endif

        this->main_fn = main_fn;

        // Parameters for tracedump function are as follows:
        // 1: Address of control flow map buffer
        // 2: Size of buffer in bytes
        SigDecl s = {.params = {WASM_TYPE_I32, WASM_TYPE_I32}, .results = {}};
        ImportInfo iminfo = {.mod_name = "allspark:trace",
                             .member_name = "prog_tracedump"};
        ImportDecl *import_decl = module.add_import(iminfo, s);
        this->tracedump_fn = import_decl->desc.func;

        this->pc_bits = pc_bits;
        this->fnidx_bits = fnidx_bits;
    }

    void reset_current_fn(FuncDecl *func) {
        this->lc_allocator =
            LocalAllocator(func, {{WASM_TYPE_I32, 2}, {WASM_TYPE_I64, 1}});
        this->curr_fn = func;
    }
};

// Encode the function index and source pc of instruction into packed high
// bits of 64-bit value
inline uint64_t get_fn_srcpc_shift(WasmModule &module,
                                   TraceInstrumentData &trace_data,
                                   InstBasePtr &instptr) {
    // We subtract 1 from function index since we insert an import
    return ((uint64_t)instptr->getOrigPc() << 24) |
           (((uint64_t)module.getFuncIdx(trace_data.curr_fn) - 1) << 48);
}

// Instructions to record ctrlflow path into vector in memory
// To use this, control flow target is expected on top of the stack
InstBuilder ctrlflow_recorder(WasmModule &module,
                              TraceInstrumentData &trace_data,
                              InstBasePtr &instptr,
                              bool is_call_indirect = false) {

    uint64_t br_fn_srcpc_shift =
        get_fn_srcpc_shift(module, trace_data, instptr);

    auto lc_imap = trace_data.lc_allocator.get_idx_map();
    auto i32_locals = lc_imap[WASM_TYPE_I32];
    auto i64_locals = lc_imap[WASM_TYPE_I64];

    InstBuilder builder = {};
    builder.push({
        // Control flow target
        INST(LocalSetInst(i64_locals[0])),
    // Base address to write target entry
#if USE_GLOBALS_TRACE != 0
        INST(GlobalGetInst(trace_data.ctrlflow_vector)),
        INST(GlobalGetInst(trace_data.ctrlflow_bytes)),
#else
        INST(I32ConstInst(trace_data.ctrlflow_vector_start)),
#if USE_ATOMICS != 0
        // Increment and get with atomic
        INST(I32ConstInst(0)),
        INST(I32ConstInst(8)),
        INST(I32AtomicRmwAddInst(2, trace_data.ctrlflow_bytes_addr,
                                 module.getMemory(0))),
#else
        // Load and save old ctrlflow_bytes
        INST(I32ConstInst(0)),
        INST(I32LoadInst(2, trace_data.ctrlflow_bytes_addr,
                         module.getMemory(0))),
        INST(LocalTeeInst(i32_locals[1])),
        // Address to writeback
        INST(I32ConstInst(0)),
        // Increment ctrlflow_bytes
        INST(LocalGetInst(i32_locals[1])),
        INST(I32ConstInst(8)),
        INST(I32AddInst()),
        // Storeback
        INST(I32StoreInst(2, trace_data.ctrlflow_bytes_addr,
                          module.getMemory(0))),
#endif
#endif
        // Base Address for writing entry
        INST(I32AddInst()),
        // Value to write -- Pack br_pc, target
        // For call indirect, this is the last argument in function
        is_call_indirect
            ? INST(LocalGetInst(trace_data.curr_fn->sig->params.size() - 1))
            : INST(I64ConstInst(br_fn_srcpc_shift)),
        INST(LocalGetInst(i64_locals[0])),
        INST(I64OrInst()),
        // Write to memory
        INST(I64StoreInst(3, 0, module.getMemory(0))),
#if USE_GLOBALS_TRACE != 0
        // Increment ctrlflow_bytes if using globals
        INST(GlobalGetInst(trace_data.ctrlflow_bytes)),
        INST(I32ConstInst(8)),
        INST(I32AddInst()),
        INST(GlobalSetInst(trace_data.ctrlflow_bytes)),
#endif
    });
    return builder;
};

void local_cfscope_handler(WasmModule &module, ScopeBlock &scope,
                           TraceInstrumentData &trace_data) {

    static std::vector<ScopeBlock *> cfscope_stack;

    cfscope_stack.push_back(&scope);
    // Depth-first recursion of subscopes
    for (auto &subscope : scope.outer_subscopes) {
        // TRACE("[PC: %u] Recurse subscope\n",
        // (*(subscope->start))->getOrigPc());
        local_cfscope_handler(module, *subscope, trace_data);
    }

    TRACE("Processing cfscope [s:%u, e:%u]\n", (*(scope.start))->getOrigPc(),
          (*(scope.end))->getOrigPc());

    switch (scope.get_scope_type()) {
    case IF:
    case IFWELSE:
    case ELSE: {
        ERR("Cannot handle IF/ELSE opcodes yet; terminate\n");
        throw std::runtime_error("IF/ELSE opcode not supported");
        break;
    }
    default: {
    }
    }

    auto next_outer_subscope = scope.outer_subscopes.begin();

    for (auto institr = scope.start; institr != scope.end; ++institr) {
        InstBasePtr instptr = *institr;
        uint16_t opcode = instptr->getOpcode();
        uint16_t pc = instptr->getOrigPc();

        InstBuilder builder = {};

        auto get_br_target_pc = [](uint32_t depth) -> uint32_t {
            uint32_t stack_idx = cfscope_stack.size() - 1 - depth;
            ScopeBlock &target = *cfscope_stack[stack_idx];
            // Target on stack
            if (target.get_scope_type() == LOOP) {
                return (*(target.start))->getOrigPc();
            } else {
                return (*(target.end))->getOrigPc();
            }
        };

        auto br_insert = [&module, &trace_data, &pc, &institr, &instptr,
                          &builder, &get_br_target_pc]() {
            std::shared_ptr<ImmLabelInst> br_inst =
                std::static_pointer_cast<ImmLabelInst>(instptr);
            uint32_t target_pc = get_br_target_pc(br_inst->getLabel());
            // Target on stack
            builder.push_inst(I64ConstInst(target_pc));
            auto cf_builder = ctrlflow_recorder(module, trace_data, instptr);
            builder.splice(cf_builder);
            builder.splice_into(trace_data.curr_fn->instructions, institr);
        };
        auto br_if_insert = [&module, &trace_data, &pc, &institr, &instptr,
                             &builder, &get_br_target_pc]() {
            std::shared_ptr<ImmLabelInst> br_if_inst =
                std::static_pointer_cast<ImmLabelInst>(instptr);
            uint32_t succ_pc = get_br_target_pc(br_if_inst->getLabel());
            uint32_t fail_pc = (*std::next(institr))->getOrigPc();
            auto i32_locals =
                trace_data.lc_allocator.get_type_vector(WASM_TYPE_I32);
            builder.push(
                {// Save br_cond
                 INST(LocalTeeInst(i32_locals[0])), INST(I64ConstInst(succ_pc)),
                 INST(I64ConstInst(fail_pc)), INST(LocalGetInst(i32_locals[0])),
                 INST(SelectInst())});
            auto cf_builder = ctrlflow_recorder(module, trace_data, instptr);
            builder.splice(cf_builder);
            builder.splice_into(trace_data.curr_fn->instructions, institr);
        };
        auto br_table_insert = [&module, &trace_data, &pc, &institr, &instptr,
                                &builder, &get_br_target_pc]() {
            std::shared_ptr<ImmLabelsInst> br_table_inst =
                std::static_pointer_cast<ImmLabelsInst>(instptr);
            auto labels = br_table_inst->getLabels();
            // Get the number of arms
            uint32_t max_idx = *std::max_element(labels.begin(), labels.end());
            max_idx = std::max(max_idx, br_table_inst->getDefLabel());
            //
            SigDecl i64_ret_sigdecl = {.params = {},
                                       .results = {WASM_TYPE_I64}};
            SigDecl *block_sig = module.add_sig(i64_ret_sigdecl, false);
            uint32_t block_sig_idx = module.getSigIdx(block_sig);

            auto i32_locals =
                trace_data.lc_allocator.get_type_vector(WASM_TYPE_I32);
            // Store dynamic branch site in local
            builder.push_inst(LocalTeeInst(i32_locals[0]));
            for (int i = 0; i < max_idx + 1; i++) {
                builder.push_inst(BlockInst(block_sig_idx));
            }
            builder.push({INST(BlockInst(block_sig_idx)), INST(I64ConstInst(0)),
                          INST(LocalGetInst(i32_locals[0])),
                          INST(ImmLabelsInst(*br_table_inst)),
                          INST(EndInst())});
            for (int i = 0; i < max_idx + 1; i++) {
                uint32_t target_pc = get_br_target_pc(i);
                // Target on stack
                builder.push({INST(I64ConstInst(target_pc)),
                              INST(BrInst(max_idx - i)), INST(EndInst())});
            }
            auto cf_builder = ctrlflow_recorder(module, trace_data, instptr);
            builder.splice(cf_builder);
            builder.splice_into(trace_data.curr_fn->instructions, institr);
        };

        auto call_indirect_insert = [&module, &trace_data, &pc, &institr,
                                     &instptr, &builder]() {
            auto call_idr_inst =
                std::static_pointer_cast<ImmSigTableInst>(instptr);
            auto i32_locals =
                trace_data.lc_allocator.get_type_vector(WASM_TYPE_I32);

            builder.push({
                INST(LocalSetInst(i32_locals[0])),
                // Source information for call_indirect
                // Wrapper handles target information
                INST(I64ConstInst(
                    get_fn_srcpc_shift(module, trace_data, instptr))),
                INST(LocalGetInst(i32_locals[0])),
            });

            SigDecl new_sig(*(call_idr_inst->getSig()));
            new_sig.params.push_back(WASM_TYPE_I64);
            // Update call_indirect type check
            call_idr_inst->setSig(module.add_sig(new_sig, false));

            builder.splice_into(trace_data.curr_fn->instructions, institr);
        };

        TRACE("[PC: %u] [OP: %s]\n", pc, opcode_table[opcode].mnemonic);
        if ((next_outer_subscope != scope.outer_subscopes.end()) &&
            (institr == (*next_outer_subscope)->start)) {
            TRACE("Found start of a subscope; Skip\n");
            institr = (*next_outer_subscope)->end;
            std::advance(next_outer_subscope, 1);
            continue;
        }

        // Monitor before instructions that modify control flow dynamically
        switch (opcode) {
        case WASM_OP_BR: {
            br_insert();
            break;
        }
        case WASM_OP_BR_IF: {
            br_if_insert();
            break;
        }
        case WASM_OP_BR_TABLE: {
            br_table_insert();
            break;
        }
        case WASM_OP_CALL_INDIRECT: {
            call_indirect_insert();
            break;
        }
        default: {
        }
        }
    }

    cfscope_stack.pop_back();
}

void insert_tracedump(WasmModule &module, TraceInstrumentData &trace_data,
                      InstItr institr) {
    InstBuilder builder = {};
    builder.push({
#if USE_GLOBALS_TRACE != 0
        INST(GlobalGetInst(trace_data.ctrlflow_vector)),
        INST(GlobalGetInst(trace_data.ctrlflow_bytes)),
#else
        INST(I32ConstInst(trace_data.ctrlflow_vector_start)),
        INST(I32ConstInst(0)),
        INST(I32LoadInst(2, trace_data.ctrlflow_bytes_addr,
                         module.getMemory(0))),
#endif
        INST(CallInst(trace_data.tracedump_fn)),
    // Clear the globals
#if USE_GLOBALS_TRACE != 0
        INST(I32ConstInst(0)),
        INST(GlobalSetInst(trace_data.ctrlflow_bytes)),
#else
        INST(I32ConstInst(0)),
        INST(I32ConstInst(0)),
        INST(I32StoreInst(2, trace_data.ctrlflow_bytes_addr,
                          module.getMemory(0))),
#endif
    });
    builder.splice_into(trace_data.curr_fn->instructions, institr);
};

// Main Allspark Trace Instrumenter
void allspark_trace_instrument(WasmModule &module, std::string entry_export) {
    printf("Allspark Trace Instrumentation\n");
    printf("Entry Export: %s\n", entry_export.c_str());

    auto exp = module.find_export(entry_export);
    if (!exp || (exp->kind != KIND_FUNC)) {
        ERR("Entry func export \'%s\' not found\n", entry_export.c_str());
        return;
    }

    // Get the main function
    FuncDecl *main_fn = exp->desc.func;

    // Increase memory a lot to store the vectors
    uint32_t old_page_count = module.getMemory(0)->add_pages(16384);

    // First scan functions to see how to pack function, pc and target together
    uint32_t max_pc = 0;
    for (auto &func : module.Funcs()) {
        if (module.isImport(&func)) {
            continue;
        }
        max_pc = std::max((func.instructions.back())->getOrigPc(), max_pc);
    }
    uint32_t max_pc_bits = std::ceil(log2(max_pc));
    uint32_t max_fn_bits = std::ceil(log2(module.Funcs().size()));
    printf("Max PC bits: %u; Max Function Bits: %u\n", max_pc_bits,
           max_fn_bits);
    // Assert that we can pack function (16b), and two pcs (24b each)
    // together in 64b value
    assert(max_fn_bits <= 16 && max_pc_bits <= 24);

    // We marshall the 64-bit <fn-idx><src-pc> packing
    std::set<FuncDecl *> funcref_wrappers =
        module.funcref_wrap({WASM_TYPE_I64});

    TraceInstrumentData trace_data(module, main_fn,
                                   old_page_count * WASM_PAGE_SIZE, max_pc_bits,
                                   max_fn_bits);

    // Trace functions through generated scopes
    for (auto &func : module.Funcs()) {
        if (module.isImport(&func) || funcref_wrappers.contains(&func)) {
            continue;
        }
        ScopeList main_scopes = module.gen_scopes_from_instructions(&func);
        ScopeBlock main_scope = main_scopes.front();

        trace_data.reset_current_fn(&func);
        local_cfscope_handler(module, main_scope, trace_data);
    }

    // Add call indirect target recording
    for (auto &func : funcref_wrappers) {
        trace_data.reset_current_fn(func);

        InstBuilder builder = {};
        uint32_t target_func_idx = 0xFFFFFFFF;
        for (auto &inst : func->instructions) {
            // Get the target function index of the call_indirect wrapper
            if (inst->getOpcode() == WASM_OP_CALL) {
                auto call_inst = std::static_pointer_cast<ImmFuncInst>(inst);
                // Offset function index here as well
                target_func_idx = module.getFuncIdx(call_inst->getFunc()) - 1;
            }
        }
        auto institr = func->instructions.begin();
        builder.push_inst(I64ConstInst(target_func_idx));
        auto cf_builder = ctrlflow_recorder(module, trace_data, *institr, true);
        builder.splice(cf_builder);
        builder.splice_into(trace_data.curr_fn->instructions, institr);
    }

    trace_data.reset_current_fn(trace_data.main_fn);
    // Dump vector trace at the end of the main function and returns
    for (auto institr = main_fn->instructions.begin();
         institr != main_fn->instructions.end(); ++institr) {
        InstBasePtr instptr = *institr;
        uint16_t opcode = instptr->getOpcode();
        if (opcode == WASM_OP_RETURN) {
            insert_tracedump(module, trace_data, institr);
        }
    }
    insert_tracedump(module, trace_data,
                     std::prev(main_fn->instructions.end()));
}
