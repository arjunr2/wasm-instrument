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
    GlobalDecl* opcount_buf_addr;
    FuncDecl* tracedump_fn;
    FuncDecl* curr_fn;

    TraceInstrumentData(WasmModule &module, uint32_t opcount_buf_addr) {
        GlobalDecl opcount_buf_addr_gdecl = {
            .type = WASM_TYPE_I32,
            .is_mutable = false,
            .init_expr_bytes = INIT_EXPR(I32_CONST, opcount_buf_addr),
        };
        this->opcount_buf_addr = 
            module.add_global(opcount_buf_addr_gdecl, "opcount_buf");
        
        // Parameters for tracedump function are as follows:
        // 1: Scope ID
        // 2: Address to the opcode count buffer
        SigDecl s = { .params = { WASM_TYPE_I32, WASM_TYPE_I32,  }, .results = { } };
        ImportInfo iminfo = { .mod_name = "plc-trace", .member_name = "prog_tracedump" };
        ImportDecl* import_decl = module.add_import(iminfo, s); 
        this->tracedump_fn = import_decl->desc.func;
    }

    void set_current_func(FuncDecl *fn) {
        this->curr_fn = fn;
    }
};

#ifndef CLEAR_ALGORITHM
#define CLEAR_ALGORITHM 2
#endif

void end_code_block(WasmModule &module, std::map<Opcode_t, uint64_t> &scope_opcodes,
        TraceInstrumentData &trace_data, InstList::iterator &institr, 
        uint32_t scope_id) {
    InstBuilder builder = {};
    if (!scope_opcodes.empty()) {
        uint32_t largest_opcode = 0;
        for (auto &entry : scope_opcodes) {
            builder.push ({
                INST (GlobalGetInst(trace_data.opcount_buf_addr)),
                INST (I64ConstInst(entry.second)),
                INST (I64StoreInst(3, entry.first * 8, module.getMemory(0))),
            });
            largest_opcode = (entry.first > largest_opcode) ? 
                entry.first : largest_opcode;
        }
        assert (largest_opcode < LARGEST_OPCODE);

        // Import call here
        builder.push ({
            INST (I32ConstInst(scope_id)),
            INST (GlobalGetInst(trace_data.opcount_buf_addr)),
            INST (CallInst(trace_data.tracedump_fn)),
        });
        builder.push_inst (NopInst());

        // Reset the opcode count buffer
#if CLEAR_ALGORITHM == 0
        // Individually clear each opcode count
        builder.push ({
            INST (GlobalGetInst(trace_data.opcount_buf_addr)),
            INST (I32ConstInst(0)),
            INST (I32ConstInst(largest_opcode)),
            INST (MemoryFillInst(module.getMemory(0)))
        });
#elif CLEAR_ALGORITHM == 1
        // Bulk clear the entire buffer
        for (auto &entry : scope_opcodes) {
            builder.push ({
                INST (GlobalGetInst(trace_data.opcount_buf_addr)),
                INST (I64ConstInst(0)),
                INST (I64StoreInst(3, entry.first * 8, module.getMemory(0))),
            });
        }
#elif CLEAR_ALGORITHM == 2
        // Hybrid: Bulk clear the first range of buffer 
        // (base opcodes are more frequent) and individually clear the rest
        for (auto &entry : scope_opcodes) {
            if (entry.first >= LARGEST_BASE_OPCODE) {
                builder.push ({
                    INST (GlobalGetInst(trace_data.opcount_buf_addr)),
                    INST (I64ConstInst(0)),
                    INST (I64StoreInst(3, entry.first * 8, module.getMemory(0))),
                });
            }
        }
        builder.push ({
            INST (GlobalGetInst(trace_data.opcount_buf_addr)),
            INST (I32ConstInst(0)),
            INST (I32ConstInst(LARGEST_BASE_OPCODE * OPCOUNT_SIZE)),
            INST (MemoryFillInst(module.getMemory(0)))
        });
#endif
        builder.splice_into(trace_data.curr_fn->instructions, institr);
        scope_opcodes.clear();
    }
}

// Recursive method to handle subscopes
void local_scope_handler(WasmModule &module, ScopeBlock &scope, 
        TraceInstrumentData &trace_data) {
    static std::map<Opcode_t, uint64_t> scope_opcodes;
    static uint32_t scope_id = 0;

    auto next_outer_subscope = scope.outer_subscopes.begin();
    // Skip block/loop scoping instructions from the count
    auto institr = scope.start;

    // Lambda to end a trace for a block
    auto end_block = [&module, &trace_data, &institr]() {
        end_code_block(module, scope_opcodes, trace_data, institr, scope_id);
        scope_id++;
    };

    for (; institr != scope.end; ++institr) {
        InstBasePtr instptr = *institr;
        uint16_t opcode = instptr->getOpcode();
        uint16_t pc = instptr->getOrigPc();
        TRACE("[PC: %u] [OP: %s]\n", instptr->getOrigPc(), opcode_table[opcode].mnemonic);
        if ((next_outer_subscope != scope.outer_subscopes.end()) && (institr == (*next_outer_subscope)->start)) {
            TRACE("Found start of subscope; Dump counts\n");
            // Encountered a subscope; it will be handled by the recursive call
            // Instrument here and skip to the end of subscope
            end_block();
            institr = (*next_outer_subscope)->end;
            std::advance(next_outer_subscope, 1);
            continue;
        }
        TRACE("Recording opcode.. %d\n", opcode);
        // Record current instruction
        uint32_t op_encoding = (opcode & 0x0FFF);
        scope_opcodes[op_encoding]++;

        // Return/branch opcodes terminate a block 
        switch (opcode) {
            case WASM_OP_RETURN: 
            case WASM_OP_BR:
            case WASM_OP_BR_IF:
            case WASM_OP_BR_TABLE:
            {
                TRACE("Found branch/return; Dump counts\n");
                end_block(); break; 
            }
            default: {}
        }
    }

    // End code block once reached the end of scope
    TRACE("End of scope; Dump counts\n");
    end_block();

    for (auto &subscope : scope.outer_subscopes) {
        TRACE("[PC: %u] Recurse subscope\n", (*(subscope->start))->getOrigPc());
        local_scope_handler(module, *subscope, trace_data);
    }
}

// Main PLC Trace function
void plc_trace_instrument (WasmModule &module) {
    printf("PLC Trace Instrumentation\n");

    // Memory address for the bytecode buffer
    // Size = 12 bits for opcode (2^12) x u64 (2^3 bytes) = 2^15 bytes
    uint32_t old_page_count = add_pages(module.getMemory(0), 2);
    uint32_t opcount_buf_addr = old_page_count * WASM_PAGE_SIZE;

    TraceInstrumentData trace_data(module, opcount_buf_addr);

    for (auto &func: module.Funcs()) {
        if (module.isImport(&func)) {
            continue;
        }
        trace_data.set_current_func(&func);
        printf("\nFunc Idx: %d\n", module.getFuncIdx(&func));
        std::map<Opcode_t, uint64_t> scope_opcodes;
        ScopeList main_scopes = module.gen_scopes_from_instructions(&func);
        ScopeBlock main_scope = main_scopes.front();
        local_scope_handler(module, main_scope, trace_data);
    }
}