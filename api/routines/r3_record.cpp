#include "routine_common.h"
#include "r3_common.h"
#include <cstring>
#include <cassert>
#include <ranges>

#define INST(inv) InstBasePtr(new inv)


// Memory operation trace-relevant properties
struct MemopInstInfo {
    SigDecl *sig;
	LocalVal ret_lc;
	MemopProp prop;
	uint32_t offset;
	LocalVal baseaddr_lc;
    // For prop.size = -1, the width is dynamically stored in this local
	LocalVal accwidth_lc; 
    // Opcode is kept for size=0, which handles special cases
	uint16_t opcode; 
    // Accesses that enforce implicit synchronization
	bool is_sync; 

    public:
        bool has_retval() {
            // Only support single return value so far
            assert(sig->results.size() <= 1);
            return sig->results.size() > 0;
        }
} ;

// Call instruction trace-relevant properties
struct CallInstInfo {
	LocalVal ret_lc;
    std::vector<LocalVal> args;
	uint16_t opcode;
	FuncDecl *target;
	RecordInterface::CallID call_id;

    public:
        bool has_retval() {
            // Only support single return value so far
            assert(target->sig->results.size() <= 1);
            return target->sig->results.size() > 0;
        }
        void move_into(LocalVal &ret_lc, std::vector<LocalVal> &args, uint16_t opcode,
                FuncDecl *target, RecordInterface::CallID call_id) {
            this->ret_lc = ret_lc;
            this->args = std::move(args);
            this->opcode = opcode;
            this->target = target;
            this->call_id = call_id;
        }
};

// Type of instrumentation required for the instruction
struct InstrumentType {
	bool lockless;
	bool lock_futex;
	bool pre_insert_trace;
	bool force_trace;
};

// Generalized record operation properties
enum RecordOpType { MEM_OP, CALL_OP };
struct RecordInstInfo {
    RecordOpType optype;
	MemopInstInfo Memop;
	CallInstInfo Call;
};

// Module-relevant context
struct ModuleContext {
    WasmModule &module;
    MemoryDecl *def_mem;
    MemoryDecl *record_mem;
    std::set<FuncDecl*> &funcref_wrappers;
};

// A union type to represent configurable static/dynamic instrumentation vars 
// uniformly
union InstrumentVarMode {
    // Statically determined (default case)
    uint32_t constant;
    // Dynamically determined (e.g. call_indirect)
    LocalVal lc;
};

// Instruction-relevant context
struct InstructionContext {
    InstBasePtr &instruction;
    FuncDecl* func;
    bool is_dynamic_callsite;
    InstrumentVarMode access_idx;

public:
    InstBasePtr construct_access_idx_inst() {
        if (is_dynamic_callsite) {
            return INST(LocalGetInst(access_idx.lc.idx));
        } else {
            return INST(I32ConstInst(access_idx.constant));
        }
    }
};

// Multi-threading relevant context
struct ThreadRecordInfo {
	FuncDecl *lock_fn;
	FuncDecl *unlock_fn;
};

// Access to functions with non-generic behavior and require "special" handling 
struct CallMaps {
    std::map<FuncDecl*, std::string> &specialized;
    std::map<FuncDecl*, std::string> &lockless;

    // Check (and return element) if function is specialized
    std::pair<bool, std::string> is_specialized(FuncDecl *fn) {
        auto x = specialized.find(fn);
		bool found = (x != specialized.end());
		return std::make_pair(found, found ? x->second : std::string());
    }

    // Check (and return element) if function is lockless
    std::pair<bool, std::string> is_lockless(FuncDecl *fn) {
        auto x = lockless.find(fn);
		bool found = (x != lockless.end());
		return std::make_pair(found, found ? x->second : std::string());
    }
};


// Quick cached-access of sig indices (since list indexing is expensive)
static std::pair<uint32_t, SigDecl*> get_sig_idx_cache(WasmModule &module, SigDecl sig) {
    static std::unordered_map<SigDecl, 
        std::pair<uint32_t, SigDecl*>, SigDeclHash> sig_idx_cache;

    auto sig_entry = sig_idx_cache.find(sig);
    if (sig_entry == sig_idx_cache.end()) {
        SigDecl *sig_ptr = module.add_sig(sig, false);
        uint32_t idx = module.getSigIdx(sig_ptr);
        auto ret = std::make_pair(idx, sig_ptr);
        sig_idx_cache[sig] = ret;
        return ret;
    } else {
        return sig_entry->second;
    }
}

/** Common instruction instrumentation patterns **/
// Instructions to conditionally grab a lock if local_val == val
static InstBuilder builder_push_ifeq_lock(
        InstBuilder &builder, ThreadRecordInfo &thread_info, 
        LocalVal &local_val, uint32_t val) {
    builder.push ({
        INST (BlockInst(-64)),
        INST (LocalGetInst(local_val.idx)),
        INST (I32ConstInst(0x7f)),
        INST (I32AndInst()),
        INST (I32ConstInst(val)),
        INST (I32NeInst()),
        INST (BrIfInst(0)),
        /* Lock if val == local_val.idx */ 
        INST (CallInst(thread_info.lock_fn)),
        INST (EndInst())
    });
    return builder;
}

// Instructions to typecast any type to I64
static InstBuilder builder_push_i64_extend(InstBuilder &builder, 
        wasm_type_t type) {
    switch (type) {
        case WASM_TYPE_I32: builder.push_inst (I64ExtendI32UInst()); break;
        case WASM_TYPE_I64: ; break;
        case WASM_TYPE_F32: builder.push({
            INST (I32ReinterpretF32Inst()),
            INST (I64ExtendI32UInst())
        }); break;
        case WASM_TYPE_F64: builder.push_inst (I64ReinterpretF64Inst()); break;
		default: { 
            ERR("R3-Record-Error: Unsupported type %d for I64 Extend\n", type); 
        }
    }
    return builder;
}

// Generate an <type>_NE instruction
static InstBasePtr ne_type_inst(wasm_type_t type) {
    switch (type) {
        case WASM_TYPE_I32: return INST(I32NeInst());
        case WASM_TYPE_I64: return INST(I64NeInst());
        case WASM_TYPE_F32: return INST(F32NeInst());
        case WASM_TYPE_F64: return INST(F64NeInst());
        default: { ERR("R3-Record-Error: Unsupported type %d for NEQ\n", type); }
    }
    return NULL;
}


// Generate a instruction to store <type> sized data back to (recorded) memory
static InstBasePtr writeback_inst(uint32_t size, MemoryDecl *mem) {
    switch (size) {
        case 1: return INST(I64Store8Inst(0, 0, mem));
        case 2: return INST(I64Store16Inst(0, 0, mem));
        case 4: return INST(I64Store32Inst(0, 0, mem));
        case 8: return INST(I64StoreInst(0, 0, mem));
        default: { ERR("R3-Record-Error: Unsupported size %d for writeback\n", size); }
    }
    return NULL;
}
/** */

// Helper method to allocate a return value local
static std::pair<bool, LocalVal> allocate_retval_local(LocalAllocator &lc_allocator, 
        SigDecl *sig) {
    LocalVal ret_lc;
    bool has_ret = sig->results.size() > 0;
    if (has_ret) {
        // Single return value for now
        assert (sig->results.size() == 1);
        ret_lc = lc_allocator.allocate(sig->results.front());
    }
    return std::make_pair(has_ret, ret_lc);
}

// Helper method to non-destructively save list of typed stack arguments 
// to new locals (non-destructive)
// Updates the builder, and also returns the locals allocated
// New locals are ordered in the same order as `values`
static std::vector<LocalVal> 
save_stack_args(InstBuilder &builder, LocalAllocator &lc_allocator, 
        typelist &values) {
    int num_params = values.size();
    std::vector<LocalVal> local_vals_allocated(num_params);

    int i = num_params - 1;
    for (auto xt = values.rbegin(); 
            xt != values.rend(); xt++, i--) {
        LocalVal newlc = lc_allocator.allocate(*xt);
        // Tee only the last value; set otherwise
        if (i > 0) {
            builder.push_inst(LocalSetInst(newlc.idx));
        } else {
            builder.push_inst(LocalTeeInst(newlc.idx));
        }
        local_vals_allocated[i] = newlc;
    }
    for (i = 1; i < local_vals_allocated.size(); i++) {
        builder.push_inst(LocalGetInst(local_vals_allocated[i].idx));
    }
    return local_vals_allocated;
}

// Instructions to duplicate a memory operation to shadow memory
// `default_updater` is used to perform updates on specific record fields 
//  based on the memory operation type 
template <typename ImmT>
InstBuilder setup_memop_duplicate (InstBasePtr &instruction,
    LocalAllocator &lc_allocator,
    ModuleContext &modctx, MemopInstInfo &recinfo, 
    void (*default_updater)(ImmT* new_inst, ModuleContext &modctx, 
        std::vector<LocalVal> &local_vals_allocated, 
        LocalVal &accwidth_lc, uint32_t &mem_offset),
    bool is_sync_op = false) {

    uint16_t opcode = instruction->getOpcode();
    MemopProp memop_prop = memop_inst_table[opcode];

    // Memory operation signature
    SigDecl memop_sigdecl = get_sigdecl_from_cdef(memop_prop.sig);
    auto memop_sigpair = get_sig_idx_cache(modctx.module, memop_sigdecl);
    SigDecl* memop_sig = memop_sigpair.second;

    // Block signature
    SigDecl memop_block_sigdecl = { 
        .params = memop_sig->params, .results = memop_sig->params 
    };
    auto memop_block_sigpair = get_sig_idx_cache(modctx.module, memop_block_sigdecl);
    uint32_t memop_block_sigidx = memop_block_sigpair.first;

    uint32_t num_params = memop_sig->params.size();

    auto retlc_pair = allocate_retval_local(lc_allocator, memop_sig);
    bool has_ret = retlc_pair.first;
    LocalVal ret_lc = retlc_pair.second;

    InstBuilder builder = {};
    
    builder.push_inst(BlockInst(memop_block_sigidx));
    // Save the stack arguments so we can duplicate them
    std::vector<LocalVal> local_vals_allocated = 
        save_stack_args(builder, lc_allocator, memop_sig->params);
    // Duplicate the arguments for memop
    for (auto xt = local_vals_allocated.begin(); xt != local_vals_allocated.end(); xt++) {
        builder.push_inst(LocalGetInst(xt->idx));
    }

    // Default values of record for memory operation (can be updated using `default_updater`)
	uint32_t mem_offset = 0;
    LocalVal accwidth_lc;
    // Base address is always at the bottom of the stack operation (first param)
    LocalVal baseaddr_lc = local_vals_allocated.front();

	std::shared_ptr<ImmT> mem_inst = static_pointer_cast<ImmT>(instruction);     

    ImmT *new_meminst = new ImmT(*mem_inst);
    // Custom update for specific memory operations
    default_updater(new_meminst, modctx, local_vals_allocated, accwidth_lc, mem_offset);

    builder.push_inst(InstBasePtr(new_meminst));
    if (has_ret) {
        builder.push_inst(LocalSetInst(ret_lc.idx));
    }
    builder.push_inst(EndInst());

    // Populate the relevant recording info to send to tracing function
    recinfo = { .sig = memop_sig, .ret_lc = ret_lc, .prop = memop_prop, 
        .offset = mem_offset, 
        .baseaddr_lc = baseaddr_lc, .accwidth_lc = accwidth_lc, 
        .opcode = opcode, .is_sync = is_sync_op };

    return builder;
}

// Setup for ImmMemargLaneidxInst instructions
InstBuilder setup_memarg_laneidx_instgen (InstructionContext &instctx, 
        LocalAllocator &lc_allocator,
        ModuleContext &modctx, MemopInstInfo &recinfo) {

    uint16_t opcode = instctx.instruction->getOpcode();
	ERR("R3-Record-Error: Cannot support opcode %04X (%s) yet\n", 
        opcode, opcode_table[opcode].mnemonic);

    auto updater = [](ImmMemargLaneidxInst* new_inst, ModuleContext &modctx, 
            auto &local_vec, LocalVal &accwidth_lc, auto &_c) {
        new_inst->setMemory(modctx.record_mem);
    };
    // Do not duplicate for now..
    return {};
}


// Setup for ImmMemoryInst instructions
InstBuilder setup_memory_instgen (InstructionContext &instctx, 
        LocalAllocator &lc_allocator,
        ModuleContext &modctx, MemopInstInfo &recinfo) {

    auto updater = [](ImmMemoryInst* new_inst, ModuleContext &modctx, 
            auto &local_vec, LocalVal &accwidth_lc, auto &_c) {
        new_inst->setMemory(modctx.record_mem);
        switch (new_inst->getOpcode()) {
            // Top of stack for MEMORY_FILL contains length of copy
            case WASM_OP_MEMORY_FILL: accwidth_lc = local_vec.back(); break;
            default: ;
        }
    };

    return setup_memop_duplicate<ImmMemoryInst>(instctx.instruction, 
        lc_allocator, modctx, recinfo, updater, false);
}


// Setup for ImmDataMemoryInst instructions
InstBuilder setup_data_memory_instgen (InstructionContext &instctx, 
        LocalAllocator &lc_allocator,
        ModuleContext &modctx, MemopInstInfo &recinfo) {

    auto updater = [](ImmDataMemoryInst* new_inst, ModuleContext &modctx, 
            auto &local_vec, LocalVal &accwidth_lc, auto &_c) {
        new_inst->setMemory(modctx.record_mem);
        // Top of stack for MEMORY_INIT contains length of copy
        accwidth_lc = local_vec.back();
    };

    return setup_memop_duplicate<ImmDataMemoryInst>(instctx.instruction, 
        lc_allocator, modctx, recinfo, updater, false);
}


// Setup for ImmMemorycpInst instructions
InstBuilder setup_memorycp_instgen (InstructionContext &instctx, 
        LocalAllocator &lc_allocator,
        ModuleContext &modctx, MemopInstInfo &recinfo) {

    auto updater = [](ImmMemorycpInst* new_inst, ModuleContext &modctx, 
            auto &local_vec, LocalVal &accwidth_lc, auto &_c) {
        new_inst->setDstMemory(modctx.record_mem);
        // Top of stack for MEMORY_COPY contains length of copy
        accwidth_lc = local_vec.back();
    };

    return setup_memop_duplicate<ImmMemorycpInst>(instctx.instruction, 
        lc_allocator, modctx, recinfo, updater, false);
}


// Setup for ImmMemargInst instructions
InstBuilder setup_memarg_instgen (InstructionContext &instctx, 
        LocalAllocator &lc_allocator,
        ModuleContext &modctx, MemopInstInfo &recinfo) {

    uint16_t opcode = instctx.instruction->getOpcode();
    bool is_sync_op = false;
    bool ignore_op = false;
    // Special cases for certain opcodes -- synchronization and ignore
    switch (opcode) {
		case WASM_OP_I32_ATOMIC_RMW_XCHG:
		case WASM_OP_I32_ATOMIC_RMW8_XCHG_U:
		case WASM_OP_I32_ATOMIC_RMW16_XCHG_U:
		case WASM_OP_I64_ATOMIC_RMW_XCHG:
		case WASM_OP_I64_ATOMIC_RMW8_XCHG_U:
		case WASM_OP_I64_ATOMIC_RMW16_XCHG_U:
		case WASM_OP_I64_ATOMIC_RMW32_XCHG_U:
		case WASM_OP_I32_ATOMIC_RMW_CMPXCHG:
		case WASM_OP_I32_ATOMIC_RMW8_CMPXCHG_U:
		case WASM_OP_I32_ATOMIC_RMW16_CMPXCHG_U:
		case WASM_OP_I64_ATOMIC_RMW_CMPXCHG:
		case WASM_OP_I64_ATOMIC_RMW8_CMPXCHG_U:
		case WASM_OP_I64_ATOMIC_RMW16_CMPXCHG_U:
		case WASM_OP_I64_ATOMIC_RMW32_CMPXCHG_U: 
            is_sync_op = true; break;
		case WASM_OP_MEMORY_ATOMIC_NOTIFY:
		case WASM_OP_MEMORY_ATOMIC_WAIT32:
		case WASM_OP_MEMORY_ATOMIC_WAIT64:
            ignore_op = true; break;
        default: 
            is_sync_op = false;
            ignore_op = false;
    }

    // Ignore the operation if it is not supported
    if (ignore_op) {
        WARN("R3-Record-Warning: Ignoring Op %d (%s)\n", opcode, 
            opcode_table[opcode].mnemonic);
        return {};
    }

    auto updater = [](ImmMemargInst* new_inst, ModuleContext &modctx, 
            auto _a, auto &_b, uint32_t &mem_offset) {
        new_inst->setMemory(modctx.record_mem);
        // Updates memory offset
        mem_offset = new_inst->getOffset();
    };

    return setup_memop_duplicate<ImmMemargInst>(instctx.instruction, 
        lc_allocator, modctx, recinfo, updater, is_sync_op);
}


// Setup for ImmFuncInst (call) instructions
InstBuilder setup_call_instgen (InstructionContext &instctx, 
        LocalAllocator &lc_allocator, CallMaps &call_maps,
		InstrumentType &instrument_type, ThreadRecordInfo &thread_info,
        ModuleContext &modctx, CallInstInfo &recinfo) {

    std::shared_ptr<ImmFuncInst> call_inst = 
        static_pointer_cast<ImmFuncInst>(instctx.instruction);
    
    // Get the target of function call
	FuncDecl *target = call_inst->getFunc();
    SigDecl *target_sig = target->sig;
	
    RecordInterface::CallID call_id = RecordInterface::SC_UNKNOWN;
    std::vector<LocalVal> args;

    // Currently only supports wali specialized calls
	std::string mod_name = "wali";
    bool internal_call = false;

    auto spec_elem = call_maps.is_specialized(target);
    auto lockless_elem = call_maps.is_lockless(target);

    auto retlc_pair = allocate_retval_local(lc_allocator, target_sig);
    bool has_ret = retlc_pair.first;
    LocalVal ret_lc = retlc_pair.second;

    InstBuilder builder = {};
    std::vector<LocalVal> local_vals_allocated;

    // Helper method to allocate args into locals
    auto allocate_arg_locals_instgen = [&lc_allocator, &local_vals_allocated, &builder, &target_sig] {
        local_vals_allocated = save_stack_args(builder, lc_allocator, target_sig->params);
    };

    if (spec_elem.first) {
        // Specialized calls
        std::string call_name = spec_elem.second;
        if (call_name == "SYS_mmap") {
            call_id = RecordInterface::SC_MMAP;
            instrument_type.force_trace = true;
        }
        else if (call_name == "SYS_writev") {
            call_id = RecordInterface::SC_WRITEV;
            allocate_arg_locals_instgen();
        }
		else if (call_name == "__wasm_thread_spawn") {
            call_id = RecordInterface::SC_THREAD_SPAWN;
            allocate_arg_locals_instgen();
        }
    }
	else if (lockless_elem.first) {
        // Lockless calls
        std::string call_name = lockless_elem.second;
		if (call_name == "SYS_futex") {
            // Futex wait/wake still uses a custom lock unlike other functions
            call_id = RecordInterface::SC_FUTEX;
            instrument_type.lock_futex = true;
            allocate_arg_locals_instgen();
            /* Lock before import call only for wakes */
            builder_push_ifeq_lock(builder, thread_info,
                local_vals_allocated[1], 
                RecordInterface::FutexWake);
		}
        else if (call_name == "SYS_exit") {
            call_id = RecordInterface::SC_THREAD_EXIT;
            // Since thread exit ends execution, we need the trace output before the call
            instrument_type.pre_insert_trace = true;
            allocate_arg_locals_instgen();
        }
        else if ((call_name == "SYS_exit_group") || (call_name == "__proc_exit")) {
            call_id = RecordInterface::SC_PROC_EXIT;
            // Since proc exit ends execution, we need the trace output before the call
            instrument_type.pre_insert_trace = true;
            allocate_arg_locals_instgen();
        }
        instrument_type.lockless = true;
        instrument_type.force_trace = true;
    }
    else if (modctx.module.isImport(target) && !lockless_elem.first) {
        // Generic import calls
        call_id = RecordInterface::SC_GENERIC;
        instrument_type.force_trace = true;
    }
    else {
        // Handle return for internal calls
        internal_call = true;
    }

    uint16_t call_opcode = call_inst->getOpcode();
    // Populate the relevant recording info to send to tracing function
    recinfo.move_into(ret_lc, local_vals_allocated, call_opcode, target, call_id);

    return builder;
}



// Setup for ImmSigTable (call_indirect) instructions
// Passes additional access_idx to indirect target, which is instrumented
// to accept this during tracing
InstBuilder setup_call_indirect_instgen (InstructionContext &instctx, 
        LocalAllocator &lc_allocator, InstrumentType &instrument_type, 
        ModuleContext &modctx) {

    std::shared_ptr<ImmSigTableInst> call_idr_inst = 
        static_pointer_cast<ImmSigTableInst>(instctx.instruction);

    InstBuilder builder = {};
    // Temporary local for popping dynamic function index from stack
    LocalVal dyn_func_idx_lc = lc_allocator.allocate(WASM_TYPE_I32);

    // Access index
    builder.push ({
        INST (LocalSetInst(dyn_func_idx_lc.idx)),
        INST (I32ConstInst(instctx.access_idx.constant)),
        INST (LocalGetInst(dyn_func_idx_lc.idx))
    });
    instrument_type.lockless = true;
    instrument_type.force_trace = true;
    // Modify the signature of call_indirect target to accept access_idx
    SigDecl new_sig(*(call_idr_inst->getSig()));
    new_sig.params.push_back(WASM_TYPE_I32);
    call_idr_inst->setSig(modctx.module.add_sig(new_sig));

    return builder;
}

// Tracedump instruction generation for CALL_OP
InstBuilder callop_trace_instgen(InstructionContext &instctx, 
        LocalAllocator &lc_allocator, CallInstInfo &recinfo, 
        FuncDecl *tracedump_fn, InstrumentType &instrument_type,
		ThreadRecordInfo &thread_info, ModuleContext modctx)  {

	RecordInterface::CallID call_id = recinfo.call_id;

    InstBuilder builder = {};

    LocalVal ret_lc = recinfo.ret_lc;
    // Pre-allocated locals used
    LocalVal i64_tmp_lc = lc_allocator.allocate(WASM_TYPE_I64);
    // Allocate-on-demand locals (only in specific casseswhen return value is present)
    LocalVal i32_lc_cond_1;

	// When preinserting, we disable the return value handling
	bool has_ret = (recinfo.has_retval() && !instrument_type.pre_insert_trace);

    // Signature for logic for specialized calls
    SigDecl i64_pr_sigdecl = { 
        .params = {WASM_TYPE_I64}, .results = {WASM_TYPE_I64} 
    };
    auto i64_pr_sigpair = get_sig_idx_cache(modctx.module, i64_pr_sigdecl);

	// Instrumentation for after the call (but before trace data)
	// Default (common) case involves getting the return value
	switch (call_id) {
		case RecordInterface::SC_UNKNOWN: {
			ERR("R3-Record-Error: Call ID %d not expected\n", call_id);
			break;
		}
		// mmap requires extra operation in addition to default for generic calls
		// local_i32_1 records memory grow length
		case RecordInterface::SC_MMAP: {
            i32_lc_cond_1 = lc_allocator.allocate(WASM_TYPE_I32);
            builder.push ({
                // Check if memory grew
                INST (BlockInst(i64_pr_sigpair.first)),
                INST (LocalTeeInst(i64_tmp_lc.idx)),
                INST (LocalGetInst(i64_tmp_lc.idx)),
                INST (I64ConstInst(0)),
                INST (I64LtSInst()),
                INST (BrIfInst(0)),
                // Execute grow in shadow memory
				INST (MemorySizeInst(modctx.def_mem)),
				INST (MemorySizeInst(modctx.record_mem)),
				INST (I32SubInst()),
				INST (LocalTeeInst(i32_lc_cond_1.idx)),
				INST (MemoryGrowInst(modctx.record_mem)),
				INST (DropInst()),
                // End block
                INST (EndInst())
            });
		}
		default: {
			// Default case: Save return value if it has to handle return
			if (has_ret) {
				builder.push_inst(LocalTeeInst(ret_lc.idx));
			}
			break; 
		}
	}

	uint32_t target_func_idx = modctx.module.getFuncIdx(recinfo.target);
    builder.push ({
        // Acc Idx
        instctx.construct_access_idx_inst(),
        // Opcode
        INST (I32ConstInst(recinfo.opcode)),
        // Function Index
        INST (I32ConstInst(target_func_idx)),
        // Call ID
        INST (I32ConstInst(call_id))
    });
	// Return Value: For values without ret, it is resolved at replay gen
	if (has_ret) {
		builder.push_inst(LocalGetInst(ret_lc.idx));
		builder_push_i64_extend(builder, ret_lc.type);
	} else {
        builder.push_inst(I64ConstInst(0));
	}

	// Args for specific call
	int num_args = 0;
    // Helper method to push args for tracedump
    auto push_arglocals_i64_extend = [&lc_allocator, &builder, &recinfo, &num_args]() {
        for (int i = 0; i < num_args; i++) {
            builder.push_inst(LocalGetInst(recinfo.args[i].idx));
            builder_push_i64_extend(builder, recinfo.args[i].type);
        }
    };

	switch (call_id) {
		case RecordInterface::SC_MMAP: {
			num_args = 1;
            builder.push_inst(LocalGetInst(i32_lc_cond_1.idx));
            builder_push_i64_extend(builder, i32_lc_cond_1.type);
			break;
		}
		case RecordInterface::SC_WRITEV: {
			num_args = 3;
            push_arglocals_i64_extend();
			break;
		}
		case RecordInterface::SC_THREAD_SPAWN: {
			num_args = 2;
            push_arglocals_i64_extend();
			break;
		}
		case RecordInterface::SC_FUTEX: {
			num_args = 3;
            push_arglocals_i64_extend();
			// Lock after import call only for waits
            builder_push_ifeq_lock(builder, thread_info,
                recinfo.args[1], RecordInterface::FutexWait);
			break;
		}
		case RecordInterface::SC_THREAD_EXIT: {
			num_args = 1;
            push_arglocals_i64_extend();
			break;
		}
		case RecordInterface::SC_PROC_EXIT: {
			num_args = 1;
            push_arglocals_i64_extend();
			break;
		}
		case RecordInterface::SC_GENERIC: { break; }
		default: { ERR("R3-Record-Error: Unsupported call ID %d\n", call_id); }
	}
	// Push remaining placeholder args
	for (int i = num_args; i < 3; i++) {
		builder.push_inst (I64ConstInst(0));
	}

	// Tracedump call
	builder.push_inst (CallInst(tracedump_fn));

	if (instrument_type.lock_futex) {
		// Unlocks for both wait and wakes
		builder.push_inst (CallInst(thread_info.unlock_fn));
	}
	
	return builder;
}



InstBuilder memop_trace_instgen(InstructionContext &instctx, 
        LocalAllocator &lc_allocator, MemopInstInfo &recinfo, 
        FuncDecl *tracedump_fn, ModuleContext &modctx) {

	InstBasePtr neq;
	InstBuilder builder = {};

	uint16_t opcode = instctx.instruction->getOpcode();

    // Return value local from shadow memory operation
	LocalVal shadow_retval_lc = recinfo.ret_lc;
    // Pre-allocated locals used
    LocalVal differ_lc = lc_allocator.allocate(WASM_TYPE_I32);
    LocalVal full_addr_lc = lc_allocator.allocate(WASM_TYPE_I32);
    // Allocate-on-demand locals (only when return value is present)
    LocalVal main_retval_lc;
    LocalVal i64_main_retval_lc;
    LocalVal i64_shadow_retval_lc;

	// Differ
	// Only values that return trigger a difference writeback
	if (recinfo.has_retval()) {
	    // Allocate all loads for monitoring memops with return values
        main_retval_lc = lc_allocator.allocate(shadow_retval_lc.type);
        i64_main_retval_lc = lc_allocator.allocate(WASM_TYPE_I64);
        i64_shadow_retval_lc = lc_allocator.allocate(WASM_TYPE_I64);
        // Local to store main return value
        builder.push({
            INST (LocalTeeInst(main_retval_lc.idx)),
            INST (LocalGetInst(main_retval_lc.idx)),
            INST (LocalGetInst(shadow_retval_lc.idx)),
            ne_type_inst(shadow_retval_lc.type)
        });
    } else {
		builder.push_inst(I32ConstInst(0));
	}
	builder.push_inst(LocalTeeInst(differ_lc.idx));

    builder.push({
        // Acc Idx
        instctx.construct_access_idx_inst(),
        // Opcode
        INST (I32ConstInst(opcode)),
        // Addr for memops: Save in local too
        INST (I32ConstInst(recinfo.offset)),
        INST (LocalGetInst(recinfo.baseaddr_lc.idx)),
        INST (I32AddInst()),
        INST (LocalTeeInst(full_addr_lc.idx))
    });

	// Size
	if (recinfo.prop.size != -1) {
        // Instructions for which it can be statically determined
		builder.push_inst(I32ConstInst(recinfo.prop.size));
	} else {
        // Instructions for which it can only be dynamically determined
		builder.push_inst(LocalGetInst(recinfo.accwidth_lc.idx));
	}

	if (recinfo.has_retval()) {
        builder.push({
            // Main-Memory Load Value with storeback to shadow
            INST (BlockInst(-64)),
            INST (LocalGetInst(differ_lc.idx)),
            INST (I32EqzInst()),
            INST (BrIfInst(0)),
            // Logic if differ != 0; i.e, writeback to loaded value from 
            // main memory to shadow memory
            INST (LocalGetInst(full_addr_lc.idx)),
            INST (LocalGetInst(main_retval_lc.idx)),
        });
        builder_push_i64_extend(builder, main_retval_lc.type);
        builder.push_inst (LocalTeeInst(i64_main_retval_lc.idx));

        builder.push ({
            // Store back main memory retval value to shadow memory
            writeback_inst(memop_inst_table[opcode].size, modctx.record_mem),
		    // Expected shadow memory load value 
            INST (LocalGetInst(shadow_retval_lc.idx))
        });
        builder_push_i64_extend(builder, shadow_retval_lc.type);
        builder.push ({
            INST (LocalSetInst(i64_shadow_retval_lc.idx)),
            INST (EndInst())
        });
        builder.push ({
            // Main memory load value
            INST (LocalGetInst(i64_main_retval_lc.idx)),
            // Shadow memory load value
            INST (LocalGetInst(i64_shadow_retval_lc.idx))
        });
	} else {
        builder.push ({
            INST (I64ConstInst(0)),
            INST (I64ConstInst(0))
        });
	}

    builder.push ({
        // Implicit Sync operation? 
        INST (I32ConstInst(recinfo.is_sync)),
        // Tracedump call
        INST (CallInst(tracedump_fn))
    });

	return builder;
}


#define SETUP_INVOKE(ty) ({ \
    auto xres = setup_##ty##_instgen(instctx, lc_allocator, modctx, record.Memop); \
    record.optype = MEM_OP; \
    xres; \
})

// Main R3 Record function
void r3_record_instrument (WasmModule &module) {
	// Engine callback for tracing
	FuncDecl* trace_fns[NUM_RECORD_IMPORTS];
	for (int i = 0; i < NUM_RECORD_IMPORTS; i++) {
		trace_fns[i] = add_r3_import_function(module, record_imports[i]);
	}
	FuncDecl* memop_tracedump_fn = trace_fns[0];
	FuncDecl* call_tracedump_fn = trace_fns[1];

	MemoryDecl *def_mem = module.getMemory(0);
	FuncDecl *mutex_funcs[2];
	uint32_t mutex_addr = (add_pages(def_mem, 1) * WASM_PAGE_SIZE);
	// Create custom mutex lock/unlock functions
	create_mutex_funcs(module, mutex_addr, mutex_funcs);

    ThreadRecordInfo thread_record_info = { 
        .lock_fn = mutex_funcs[0], 
        .unlock_fn = mutex_funcs[1] 
    };

	// Create new memory: Must happen after adding all instrumentation pages
	MemoryDecl new_mem = *def_mem;
	MemoryDecl *record_mem = module.add_memory(new_mem);

    // Setup for call_indirect interposition
    // During record: we marshall the access_idx and wrap the original target with a plain `call`.
    std::set<FuncDecl*> funcref_wrappers = instrument_funcref_elems(module, {WASM_TYPE_I32});

	// Generate quick lookup of ignored exported function idxs
	std::map<FuncDecl*, std::string> func_ignore_map;
	for (const char *func_name: ignored_export_funcnames) {
		if (!set_func_export_map(module, func_name, func_ignore_map)) {
			ERR("Could not find function export: \'%s\'\n", func_name);
		}
	}
	// Do not instrument the instrument-hooks or start (usually just initialization)
	func_ignore_map[module.get_start_fn()] = "__#internal#_start_function";
	func_ignore_map[thread_record_info.lock_fn] = "__#internal#_lock_instrument";
	func_ignore_map[thread_record_info.unlock_fn] = "__#internal#_unlock_instrument";

	// These import calls require extra non-generic instrumentation
	//	This still uses lock-based instrumentation 
	std::map<FuncDecl*, std::string> specialized_calls;
	for (int i = 0; i < sizeof(specialized_import_names) / sizeof(specialized_import_names[0]); i++) {
		if (FuncDecl *f = module.find_import_func("wali", specialized_import_names[i]))
			specialized_calls[f] = specialized_import_names[i];
	}
	// These import calls that are excluded from lock-based instrumentation
	std::map<FuncDecl*, std::string> lockless_calls;
	for (int i = 0; i < sizeof(lockless_import_names) / sizeof(lockless_import_names[0]); i++) {
		if (FuncDecl *f = module.find_import_func("wali", lockless_import_names[i]))
			lockless_calls[f] = lockless_import_names[i];
	}

    CallMaps call_maps = {
        .specialized = specialized_calls,
        .lockless = lockless_calls,
    };

    ModuleContext modctx = {
        .module = module,
        .def_mem = def_mem,
        .record_mem = record_mem,
        .funcref_wrappers = funcref_wrappers
    };

	uint32_t access_tracker = 1;

	// Iterate through all functions in the module
	for (auto &func : module.Funcs()) {
		// Skip ignored functions
		if ((func_ignore_map.count(&func))) {
			continue;
		}
        // For Call, all these locals are for call arg shepherding
        // For Memop, used for shadow/original loads and temporaries
        std::map<wasm_type_t, uint32_t> locals_count = {
            {WASM_TYPE_I32, 7}, {WASM_TYPE_I64, 6},
            {WASM_TYPE_F32, 3}, {WASM_TYPE_F64, 3}
        };

        LocalAllocator lc_allocator(&func, locals_count);
        // Verify the size of locals count
        assert (lc_allocator.get_local_pool_count() == 19);
		InstList &insts = func.instructions;

		// Instrumenting memory-related instructions
		for (auto institr = insts.begin(); institr != insts.end(); ++institr) {
			InstBasePtr &instruction = *institr;
			RecordInstInfo record;
			InstrumentType instrument_type = { 
                .lockless = false, 
				.lock_futex = false, 
                .pre_insert_trace = false, 
                .force_trace = false 
            };

            InstructionContext instctx = {
                .instruction = instruction,
                .func = &func,
                .is_dynamic_callsite = false,
                .access_idx = { .constant = access_tracker },
            };

            InstBuilder setup_builder = {};
            InstBuilder trace_builder = {};

            // Reset local allocators for every instruction instrumentation
            lc_allocator.reset_allocator();

			bool inc_access_tracker = true;
			switch(instruction->getImmType()) {
                case IMM_MEMARG: setup_builder = SETUP_INVOKE(memarg); break;
				case IMM_MEMARG_LANEIDX: setup_builder = SETUP_INVOKE(memarg_laneidx); break;
				case IMM_MEMORY: setup_builder = SETUP_INVOKE(memory); break;
				case IMM_DATA_MEMORY: setup_builder = SETUP_INVOKE(data_memory); break;
				case IMM_MEMORYCP: setup_builder = SETUP_INVOKE(memorycp); break;
				// Calls: Lock those to select import functions
				case IMM_FUNC: {
					setup_builder = setup_call_instgen(instctx, lc_allocator, 
                                call_maps, instrument_type, thread_record_info, 
                                modctx, record.Call); 
                    record.optype = CALL_OP;
					break;
                }
                // Indirect calls: Push access index and let instrumented call 
                // targets handle it.
                // Note: No break in this case; we don't want to increment 
                // access_tracker since funcref wrappers handle it
				case IMM_SIG_TABLE: {
                    setup_builder = setup_call_indirect_instgen(instctx, 
                            lc_allocator, instrument_type, modctx);
				}
				default: { inc_access_tracker = false; }; 
			}
			if (!setup_builder.empty() || instrument_type.force_trace) {
				auto next_institr = std::next(institr);
				// Insert instructions guarded by mutex (pre/post)
                InstBuilder trace_builder = {};

				switch (instruction->getImmType()) {
					case IMM_MEMARG_LANEIDX:
					case IMM_MEMORY:
					case IMM_MEMORYCP:
					case IMM_DATA_MEMORY:
					case IMM_MEMARG: {
                        trace_builder = memop_trace_instgen(instctx, lc_allocator, 
                            record.Memop, memop_tracedump_fn, modctx);
						break;
					}
					case IMM_FUNC: {
                        trace_builder = callop_trace_instgen(instctx,
                            lc_allocator, record.Call, call_tracedump_fn, 
                            instrument_type, thread_record_info, modctx);
						break;
					}
                    case IMM_SIG_TABLE: { 
                        break; 
                    }
					default: {
						ERR("R3-Record-Error: Missupported Imm Type %d\n", instruction->getImmType()); \
					}; 
				}

                // Insert all setup + trace instrumentation with appropriate 
                // lock/unlock guards into the module
                InstBuilder pre_builder = {};
                InstBuilder post_builder = {};

				if (!instrument_type.lockless) {
					pre_builder.push_inst (CallInst(thread_record_info.lock_fn));
				}
                pre_builder.splice(setup_builder);
				if (!instrument_type.pre_insert_trace) {
					post_builder.splice(trace_builder);
				} else {
					pre_builder.splice(trace_builder);
				}
				if (!instrument_type.lockless) {
                    post_builder.push_inst (CallInst(thread_record_info.unlock_fn));
				}

                pre_builder.splice_into(insts, institr);
                post_builder.splice_into(insts, next_institr);

				// Don't iterate over our inserted instructions
				institr = std::prev(next_institr);
			}
			if (inc_access_tracker) {
				access_tracker++;
			}
		}
	}

}

#undef SETUP_INVOKE
