#include "routine_common.h"
#include "r3_common.h"
#include <set>
#include <cassert>
#include <cstring>
#include <numeric>
#include <initializer_list>

#define INST(inv) InstBasePtr(new inv)

/* Replay Op C Structure */
typedef struct {
    int32_t addr;
    uint32_t size;
    int64_t value;
} ReplayMemStore;

typedef struct {
    uint64_t tid;
    int64_t return_val;
    uint32_t call_id;
    int64_t call_args[3];
    ReplayMemStore* stores;
    uint32_t num_stores;
    uint64_t sync_id;
} ReplayOpProp;

typedef struct {
  uint32_t access_idx;
  uint32_t func_idx;
  uint32_t implicit_sync;
  ReplayOpProp* props;
  uint32_t num_props;
  uint64_t max_tid;
} ReplayOp;

typedef struct {
    // Op-level info
    uint32_t access_idx;
    uint32_t func_idx;
    // Thread-level info
    uint32_t tid;
    // Prop-level info
    uint32_t prop_idx;
    uint32_t call_id;
    int64_t return_val;
    int64_t call_args[3];
    uint64_t sync_id;
} DebugLogCallInfo;

typedef struct {
    Opcode_t opcode;
    InstList::iterator &institr;
    InstList &insts;
    bool is_import_target;
    // Function target for call instructions
    FuncDecl *target_func;
} InstMetaInfo;

typedef struct ReplaySigMetaInfo {
    private:
        SigDecl *inst_sig;
        SigDecl *block_sig;
        int64_t block_sigidx;

    public:
        ReplaySigMetaInfo() { 
            inst_sig = NULL;
            block_sig = NULL;
            block_sigidx = -1;
        }

        inline bool has_retval() {
            // Only support single return value so far
            assert (inst_sig->results.size() <= 1);
            return inst_sig->results.size() == 1;
        }

        inline wasm_type_t get_ret_type() {
            assert (has_retval());
            return inst_sig->results.front();
        }
        inline int64_t get_block_sigidx() { return block_sigidx; }
        inline SigDecl* get_inst_sig() { return inst_sig; }
        inline SigDecl* get_block_sig() { return block_sig; }

        inline void set_inst_sig(SigDecl *s) { inst_sig = s; }
        inline void set_block_sig(SigDecl *s) { block_sig = s; }
        inline void set_block_sigidx(uint32_t ret_sig_indices[4]) {
            // Block_sigidx takes the return value for the block
            block_sigidx = has_retval() ?
                ret_sig_indices[to_wasmret(get_ret_type()) - RET_I32]:
                ret_sig_indices[4];
        }
    

} ReplaySigMetaInfo;

typedef struct {
    bool is_multithreaded;
    uint64_t num_threads;
    FuncDecl *wait_sync_id_func;
    FuncDecl *lock_func;
    FuncDecl *unlock_func;
    // i64 pointing to sync_id
    uint32_t sync_id_addr;
} ThreadReplayInfo;

typedef struct ReplayImportFuncDecl {
    FuncDecl *func;
    bool debug;

    bool operator<(const struct ReplayImportFuncDecl &other) const {
        return func < other.func;
    }
} ReplayImportFuncDecl;



static InstBasePtr get_replay_retval_inst(wasm_type_t type, int64_t val64) {
    uint32_t val32;
    float valf32;
    double valf64;
    switch (type) {
        case WASM_TYPE_I32: {
            std::memcpy(&val32, &val64, sizeof(uint32_t));
            return INST(I32ConstInst(val32));
        }
        case WASM_TYPE_I64: {
            return INST(I64ConstInst(val64));
        }
        case WASM_TYPE_F32: {
            std::memcpy(&valf32, &val64, sizeof(float));
            return INST(F32ConstInst(valf32));
        }
        case WASM_TYPE_F64: {
            std::memcpy(&valf64, &val64, sizeof(double));
            return INST(F64ConstInst(valf64));
        }
        default: {
            ERR("Invalid type %d for return value\n", type);
            return nullptr;
        }
    }
}


// Wrapped around replay import call; return nullptr if it doesn't generate call 
// Uncreated calls are recorded as unused, which is pruned at the end
static InstBasePtr create_call_to_replay_import(ReplayImportFuncDecl &replay_import,
        std::set<ReplayImportFuncDecl> &unused_callid_handlers, 
        bool gen_debug_calls) {
    // Create call for debug calls only if flags is set
    bool create_call = !(replay_import.debug) ||
        (replay_import.debug && gen_debug_calls);
    if (create_call) {
        unused_callid_handlers.erase(replay_import);
        return INST(CallInst(replay_import.func));
    } 
    else {
        return nullptr;
    }
}

// Operations to push a replay function's return value; defaults to 0xDEADBEEFULL
static inline InstBuilder& builder_push_return_value(InstBuilder &builder, 
        ReplaySigMetaInfo &sigmeta, int64_t return_value = 0xDEADBEEFULL) {
    if (sigmeta.has_retval()) {
        builder.push_inst(get_replay_retval_inst(sigmeta.get_ret_type(), return_value));
    }
    return builder;
}
// Operations to push an invalid case in switch table
static inline InstBuilder& builder_push_invalid_case(InstBuilder &builder) {
    builder.push ({
        INST (UnreachableInst()),
        INST (EndInst())
    });
    return builder;
}
// Operations to push a debug `SC_log_call` call
static InstBuilder builder_push_log_call(InstBuilder &builder, 
        InstBasePtr log_call_handler_func,
        DebugLogCallInfo log_info) {
    builder.push ({
        INST (I32ConstInst(log_info.access_idx)),
        INST (I32ConstInst(log_info.func_idx)),
        INST (I32ConstInst(log_info.tid)),
        INST (I32ConstInst(log_info.prop_idx)),
        INST (I32ConstInst(log_info.call_id)),
        INST (I64ConstInst(log_info.return_val))
    });
    for (int i = 0; i < (sizeof(log_info.call_args) / sizeof(log_info.call_args[0])); i++) {
        builder.push_inst(I64ConstInst(log_info.call_args[i]));
    }
    builder.push ({
        INST (I64ConstInst(log_info.sync_id)),
        log_call_handler_func
    });
    return builder;
}
// Operations to push sync_id incrementation for ordering prop executions
static InstBuilder builder_push_sync_id_inc(InstBuilder &builder, 
        uint32_t sync_id_addr, MemoryDecl *def_mem) {
    // Increment the sync_id 
    builder.push({
        INST (I32ConstInst(sync_id_addr)),
        INST (I64ConstInst(1)),
        INST (I64AtomicRmwAddInst(3, 0, def_mem)),
        INST (DropInst())
    });
    return builder;
}


static InstBuilder construct_single_replay_prop(int prop_arm_idx, ReplayOpProp &prop, 
        ReplaySigMetaInfo &sigmeta, bool implicit_sync, 
        MemoryDecl *def_mem,
        std::map<uint32_t, ReplayImportFuncDecl> &callid_handlers,
        std::set<ReplayImportFuncDecl> &unused_callid_handlers, 
        int64_t flags, ThreadReplayInfo &thread_replay_info,
        DebugLogCallInfo debug_log_call_info) {
    InstBuilder builder = {};
    bool gen_debug_calls = flags;

    debug_log_call_info.prop_idx = prop_arm_idx;
    debug_log_call_info.call_id = prop.call_id;
    debug_log_call_info.return_val = prop.return_val;
    for (int i = 0; i < (sizeof(prop.call_args) / sizeof(prop.call_args[0])); i++) {
        debug_log_call_info.call_args[i] = prop.call_args[i];
    }
    debug_log_call_info.sync_id = prop.sync_id;

    // Sub by 1 right now since sync_id starts with 1; this can be changed in future
    uint64_t cur_prop_sync_id = prop.sync_id - 1;
    // Wait for sync_id to match
    builder.push({
        INST (I64ConstInst(cur_prop_sync_id)),
        INST (CallInst(thread_replay_info.wait_sync_id_func))
    });

    // Generate function for debug logging
    auto log_call_import = callid_handlers.find(ReplayInterface::SC_LOG_CALL)->second;
    InstBasePtr log_call_handler_func = create_call_to_replay_import(log_call_import, 
        unused_callid_handlers, gen_debug_calls);
    if (log_call_handler_func) {
        builder_push_log_call(builder, log_call_handler_func, debug_log_call_info);
    }

    // Builder for side-effects
    InstBuilder sf_builder = {};
    // Some calls like `exit` end execution; we need to increment sync_id before calling it
    bool pre_insert_sync_inc = false;
    // Implicit sync operations don't actually enforce any props since the actual
    //  instruction is being run
    if (!implicit_sync) {
        // Return value for the arm
        builder_push_return_value(sf_builder, sigmeta, prop.return_val);
        
        // Side-effects for the arm
        // Prop import replays can fall into 3 categories:
        // 1. No side-effects
        // 2. Side-effects without host calls needed (e.g. mmap, futex)
        // 3. Side-effects with host calls required (e.g. exit, thread_create)
        bool gen_side_effects = true;
        InstBasePtr call_handler_func = INST(NopInst());
        auto callid_func_it = callid_handlers.find(prop.call_id);
        // For imports that require host calls for side-effects, get the appropriate function
        if (callid_func_it != callid_handlers.end()) {
            call_handler_func = create_call_to_replay_import(callid_func_it->second, 
                unused_callid_handlers, gen_debug_calls);
            // Generate side-effects only if the function is found
            if (call_handler_func) {
                gen_side_effects = true;
            } else {
                call_handler_func = INST(NopInst());
            }
        }
        if (gen_side_effects) {
            switch(prop.call_id) {
                case RecordInterface::SC_GENERIC: break;
                case RecordInterface::SC_MMAP: {
                    uint32_t grow_value = prop.call_args[0];
                    if (grow_value > 0) {
                        sf_builder.push ({
                            INST (I32ConstInst(grow_value)),
                            INST (MemoryGrowInst(def_mem)),
                            INST (DropInst())
                        });
                    }
                    break;
                }
                case RecordInterface::SC_WRITEV: {
                    int32_t fd = prop.call_args[0];
                    int32_t iov = prop.call_args[1];
                    uint32_t iovcnt = prop.call_args[2];
                    sf_builder.push ({
                        INST (I32ConstInst(fd)),
                        INST (I32ConstInst(iov)),
                        INST (I32ConstInst(iovcnt)),
                        call_handler_func
                    });
                    break;
                }
                case RecordInterface::SC_PROC_EXIT: 
                case RecordInterface::SC_THREAD_EXIT: {
                    int32_t status = prop.call_args[0];
                    sf_builder.push ({
                        INST (I32ConstInst(status)),
                        call_handler_func
                    });
                    pre_insert_sync_inc = true;
                    break;
                }
                case RecordInterface::SC_THREAD_SPAWN: {
                    int32_t setup_fn_ptr = prop.call_args[0];
                    int32_t thread_args_ptr = prop.call_args[1];
                    sf_builder.push ({
                        INST (I32ConstInst(setup_fn_ptr)),
                        INST (I32ConstInst(thread_args_ptr)),
                        call_handler_func
                    });
                    break;
                }
                case RecordInterface::SC_FUTEX: {
                    int32_t addr = prop.call_args[0];
                    RecordInterface::FutexOp futex_op = static_cast<RecordInterface::FutexOp>(prop.call_args[1]);
                    int32_t val = prop.call_args[2];
                    uint64_t sync_id = prop.sync_id;
                    switch (futex_op) {
                        case RecordInterface::FutexWait: {
                            sf_builder.push ({
                                INST (I32ConstInst(addr)),
                                INST (I32ConstInst(futex_op)),
                                INST (I32ConstInst(val)),
                                call_handler_func
                            });
                            break;
                        }
                        case RecordInterface::FutexWake: {
                            sf_builder.push ({
                                INST (I32ConstInst(addr)),
                                INST (I32ConstInst(futex_op)),
                                INST (I32ConstInst(val)),
                                call_handler_func
                            });
                            break;
                        }
                        default: {
                            ERR("R3-Replay-Error: Unsupported futex op %d\n", futex_op);
                        }
                    }
                    break;
                }
                default: {
                    ERR("R3-Replay-Error: Side-effects unsupported for call_id %u\n", prop.call_id);
                }
            }
        }
        // Stores for the arm
        for (int i = 0; i < prop.num_stores; i++) {
            ReplayMemStore &store = prop.stores[i];
            sf_builder.push ({
                INST (I32ConstInst(store.addr)),
                INST (I64ConstInst(store.value))
            });
            switch (store.size) {
                case 1: sf_builder.push_inst (I64Store8Inst(0, 0, def_mem)); break;
                case 2: sf_builder.push_inst (I64Store16Inst(0, 0, def_mem)); break;
                case 4: sf_builder.push_inst (I64Store32Inst(0, 0, def_mem)); break;
                case 8: sf_builder.push_inst (I64StoreInst(0, 0, def_mem)); break;
                default: { ERR("R3-Replay-Error: Unsupported size %u", store.size); }
            }
        }
    }

    if (!pre_insert_sync_inc) {
        builder.splice(sf_builder);
        builder_push_sync_id_inc(builder, thread_replay_info.sync_id_addr, def_mem);
    } else {
        builder_push_sync_id_inc(builder, thread_replay_info.sync_id_addr, def_mem);
        builder.splice(sf_builder);
    }

    return builder;
}


/* Construct the arm for a specific thread's replay operations */
static InstBuilder construct_thread_replay_props(WasmModule &module, 
    InstMetaInfo &imeta_info, int cur_tid, int begin_idx, int end_idx, 
    ReplayOpProp *props, ReplaySigMetaInfo &sigmeta,
    bool implicit_sync, MemoryDecl *def_mem,
    std::map<uint32_t, ReplayImportFuncDecl> &callid_handlers,
    std::set<ReplayImportFuncDecl> &unused_callid_handlers, 
    int64_t flags, ThreadReplayInfo &thread_replay_info, 
    DebugLogCallInfo debug_log_call_info) {

    InstBuilder builder = {};
    int num_props = end_idx - begin_idx;

    debug_log_call_info.tid = cur_tid;

    // Tracking the switch case for next prop
    static GlobalDecl prop_br_tracker_decl = {
        .type = WASM_TYPE_I32, .is_mutable = true, 
        .init_expr_bytes = INIT_EXPR (I32_CONST, 0)
    };
    GlobalDecl* prop_br_tracker = module.add_global(prop_br_tracker_decl);
    std::list<uint32_t> seq_labels(num_props);
    std::iota(seq_labels.begin(), seq_labels.end(), 0);

    // Start of prop replay unit
    builder.push_inst (BlockInst(sigmeta.get_block_sigidx()));

    for (int i = 0; i < num_props + 1; i++) {
        builder.push_inst(BlockInst(sigmeta.get_block_sigidx()));
    }
    // Switch table on specific prop with default case, which should never be used
    builder.push_inst (BlockInst(sigmeta.get_block_sigidx()));
    builder_push_return_value(builder, sigmeta);
    builder.push({
        INST (GlobalGetInst(prop_br_tracker)),
        INST (BrTableInst(seq_labels, num_props)),
        INST (EndInst())
    });
    
    // Construct each prop arm's replay operations
    for (int i = 0; i < num_props; i++) {
        InstBuilder prop_builder = construct_single_replay_prop(
            num_props - i - 1, props[begin_idx + i], sigmeta,
            implicit_sync, def_mem, callid_handlers, unused_callid_handlers, 
            flags, thread_replay_info, debug_log_call_info);
        builder.splice(prop_builder);
        if (implicit_sync) {
            for (int j = 0; j < sigmeta.get_inst_sig()->params.size(); j++) {
                builder.push_inst(LocalGetInst(j));
            }
            // Just use the same instruction for implicit sync operations
            builder.push_inst(*(imeta_info.institr));
        }
        // Branch out of the switch case
        builder.push_inst(BrInst(num_props - i));
        builder.push_inst(EndInst());
    }
    // Error case for invalid prop
    builder_push_invalid_case(builder);

    // End of prop replay unit
    builder.push({
        INST (GlobalGetInst(prop_br_tracker)),
        INST (I32ConstInst(1)),
        INST (I32AddInst()),
        INST (GlobalSetInst(prop_br_tracker)),
        INST (EndInst())
    });

    return builder;
}

/* Insert builder instructions into module, remove instrumented instruction, 
    and reset iterator */
static void replace_import_call(WasmModule &module, InstBuilder &builder,
        InstMetaInfo &imeta_info, uint32_t access_idx, 
        std::set<FuncDecl*> &remove_importfuncs, 
        ReplaySigMetaInfo &sigmeta, bool implicit_sync) {

    FuncDecl *target_func = imeta_info.target_func;
    // Setup the replay op function declaration
    char replay_func_debug_name[100];
    bool remove_importfunc = false;
    if (implicit_sync) {
        sprintf(replay_func_debug_name, "__replay_instrument_#-#-%u", access_idx);
    } else {
        ImportDecl *import_decl = module.get_import_name_from_func(imeta_info.target_func);
        sprintf(replay_func_debug_name, "__replay_instrument_%s-%s-%u", 
            import_decl->mod_name.c_str(), 
            import_decl->member_name.c_str(),
            access_idx);
        remove_importfunc = true;
    }

    FuncDecl replay_ops_func_decl = {
        .sig = sigmeta.get_inst_sig(),
        .pure_locals = wasm_localcsv_t(),
        .num_pure_locals = 0,
        .instructions = builder.get_buf()
    };
    FuncDecl* replay_ops_func = module.add_func(replay_ops_func_decl, NULL, replay_func_debug_name);
    InstList &replay_func_insts = replay_ops_func->instructions;

    // Remove import call
    InstItr next_inst = std::next(imeta_info.institr);
    imeta_info.insts.erase(imeta_info.institr);
    imeta_info.insts.insert(next_inst, INST(CallInst(replay_ops_func)));
    imeta_info.institr = std::prev(next_inst);
    if (remove_importfunc) {
        remove_importfuncs.insert(target_func);
    }
}


/* Inserts the return value and store replay operations (for all threads)
   Removes the call to the respective function
   */
static void insert_replay_op(WasmModule &module, InstMetaInfo &imeta_info, 
        ReplayOp &op, uint32_t ret_sig_indices[4], 
        std::set<FuncDecl*> &remove_importfuncs, 
        std::map<uint32_t, ReplayImportFuncDecl> &callid_handlers,
        std::set<ReplayImportFuncDecl> &unused_callid_handlers, 
        int64_t flags, ThreadReplayInfo &thread_replay_info,
        DebugLogCallInfo debug_log_call_info) {

    bool empty_replay_op = (op.num_props == 0);
    bool implicit_sync = op.implicit_sync;

    debug_log_call_info.access_idx = op.access_idx;
    debug_log_call_info.func_idx = op.func_idx;

    FuncDecl *target_func = imeta_info.target_func;

    // Replay operator signature
    ReplaySigMetaInfo sigmeta;
    if (implicit_sync) {
        if (!empty_replay_op) { assert (target_func == NULL); }
        // Implicit operations require signature of instruction
        SigDecl s = get_sigdecl_from_cdef(memop_inst_table[imeta_info.opcode].sig);
        sigmeta.set_inst_sig(module.add_sig(s, false));
    }
    else {
        // Verify that import function and the indices are consistent with record
        // before proceeding
        if (!empty_replay_op) {
            uint32_t replay_func_idx = module.getFuncIdx(target_func);
            uint32_t record_func_idx = transform_to_record_func_idx(replay_func_idx);
            assert(op.func_idx == record_func_idx);
        }
        sigmeta.set_inst_sig(target_func->sig);
    }
    // Set the block sig/sigidx used for replay function construction
    {
        SigDecl block_sig = { .params = {}, .results = {sigmeta.get_inst_sig()->results} };
        sigmeta.set_block_sig(module.add_sig(block_sig, false));
        sigmeta.set_block_sigidx(ret_sig_indices);
    }


    InstBuilder builder = {};

    if (empty_replay_op) {
        // If no props, skip replay function construction and insert a nop
        if (sigmeta.has_retval()) {
            builder.push_inst (get_replay_retval_inst(sigmeta.get_ret_type(), 0));
        } 
        builder.push_inst (EndInst());
    }
    else {
        // TID-arms range [0, max_tid] and a "max_tid + 1" for invalid tid
        uint32_t first_tid = 0;
        uint32_t num_tid_arms = op.max_tid + 2;
        uint32_t num_valid_tid_arms = num_tid_arms - 1;
        // Replay function instruction construction
        // Block for sequencing thread-specific replay operations
        if (thread_replay_info.is_multithreaded) {
            std::list<uint32_t> thread_labels(num_valid_tid_arms);
            std::iota(thread_labels.begin(), thread_labels.end(), 0);

            // Thread Replay Info: Includes an error case if tid is outside of [0, max_tid]
            for (int i = 0; i < num_tid_arms; i++) {
                builder.push_inst(BlockInst(sigmeta.get_block_sigidx()));
            }

            // Switch table on thread id with default case, which should never be used
            auto gettid_import = callid_handlers.find(ReplayInterface::SC_GETTID)->second;
            builder.push_inst (BlockInst(sigmeta.get_block_sigidx()));
            // This is for type-safety of block; should never be returned from table
            builder_push_return_value(builder, sigmeta);
            builder.push({
                create_call_to_replay_import(gettid_import, 
                    unused_callid_handlers, true),
                INST (BrTableInst(thread_labels, num_valid_tid_arms)),
                INST (EndInst())
            });

            // Construct each thread arm's replay operations
            for (int cur_tid = first_tid, i = 0; i < op.num_props; cur_tid++) {
                // Obtain the range of ops [begin_idx, end_idx) for cur_tid
                int begin_idx = i;
                while ((i < op.num_props) && (op.props[i].tid == cur_tid)) { i++; }
                int end_idx = i;
                // Assumes only single memory for now
                InstBuilder tid_props = construct_thread_replay_props(
                    module, imeta_info, cur_tid, begin_idx, end_idx, op.props, 
                    sigmeta, implicit_sync,
                    module.getMemory(0), 
                    callid_handlers, unused_callid_handlers, 
                    flags, thread_replay_info, debug_log_call_info);
                builder.splice(tid_props);
                // Branch out of the switch case
                builder.push_inst(BrInst(num_valid_tid_arms - cur_tid));
                builder.push_inst(EndInst());
            }
            // Error case for invalid tid
            builder_push_invalid_case(builder);
        }
        // Single-threaded replays can avoid gettid and tid switch table
        else {
            // Construct replay operations for single-threaded replay
            InstBuilder single_thread_props = construct_thread_replay_props(
                module, imeta_info, first_tid, 0, op.num_props, op.props, 
                sigmeta, implicit_sync, 
                module.getMemory(0), 
                callid_handlers, unused_callid_handlers, 
                flags, thread_replay_info,
                debug_log_call_info);
            builder.splice(single_thread_props);
        }
        /** */

        // End of function
        builder.push_inst (EndInst());
    }

    /* Builder instructions finished; replace call */
    replace_import_call(module, builder, imeta_info, 
        op.access_idx, remove_importfuncs, sigmeta, 
        implicit_sync);
}

/* Create function to implement synchronization of program to deterministic trace order */
/* Function waits until sync_id_addr contains param0 in it */
static FuncDecl* create_wait_sync_id_func(WasmModule &module, uint32_t sync_id_addr) {
    MemoryDecl *memory = module.getMemory(0);
    SigDecl i64p_sig_decl = { .params = {WASM_TYPE_I64}, .results = {} };
    SigDecl *i64p_sig = module.add_sig(i64p_sig_decl, false);
    uint32_t i64p_sig_idx = module.getSigIdx(i64p_sig);

    FuncDecl fn = {
        .sig = i64p_sig,
        .pure_locals = wasm_localcsv_t(),
        .num_pure_locals = 0,
        .instructions = {
            INST (LoopInst(-0x40)),
            INST (I32ConstInst(sync_id_addr)),
            INST (I64AtomicLoadInst(3, 0, memory)),
            INST (LocalGetInst(0)) /* sync_id */,
            INST (I64NeInst()),
            INST (BrIfInst(0)), /* Retry until M[sync_id_addr] != sync_id */
            INST (EndInst()),
            INST (EndInst())
        }
    };
    return module.add_func(fn, NULL, "__wait_replay_sync_id");
}


void r3_replay_instrument (WasmModule &module, void *replay_ops, 
        uint32_t num_ops, int64_t flags) {
    ReplayOp *ops = (ReplayOp *)replay_ops;

    /* We need to ensure address space consistency
    with the recorded module which adds a page for instrumentation */
    MemoryDecl *def_mem = module.getMemory(0);
    FuncDecl *mutex_funcs[2];
    uint32_t instrument_page_count = 1;
    uint32_t instrument_page_start = 
        (add_pages(def_mem, instrument_page_count) * WASM_PAGE_SIZE);
    
    // Call_indirect interpolation
    std::set<FuncDecl*> funcref_wrappers = instrument_funcref_elems(module, false);
    
    FuncDecl *last_instrumented_func = &module.Funcs().back();

    /* Create all thread-related management */
    uint32_t mutex_addr = instrument_page_start;
    uint32_t sync_id_addr = instrument_page_start + 8;
    create_mutex_funcs(module, mutex_addr, mutex_funcs);
    ThreadReplayInfo thread_replay_info = {
        .wait_sync_id_func = create_wait_sync_id_func(module, sync_id_addr),
        .lock_func = mutex_funcs[0],
        .unlock_func = mutex_funcs[1],
        .sync_id_addr = sync_id_addr,
    };
    // Get total number of threads
    // Since main thread starts only at TID=1, get max.tid (not max.tid + 1)
    for (int i = 0; i < num_ops; i++) {
        thread_replay_info.num_threads = std::max(thread_replay_info.num_threads, ops[i].max_tid);
    }
    thread_replay_info.is_multithreaded = (thread_replay_info.num_threads > 1);

    DebugLogCallInfo debug_log_call_info = { };
    /* Engine callbacks for specialized replay handlers */
    std::map<uint32_t, ReplayImportFuncDecl> callid_handlers;
    std::set<ReplayImportFuncDecl> unused_callid_handlers;
    for (int i = 0; i < NUM_REPLAY_IMPORTS; i++) {
        ReplayImportFuncDecl rep_imfunc = { 
            .func = add_r3_import_function(module, replay_imports[i]), 
            .debug = replay_imports[i].debug 
        };

        if (callid_handlers.contains(replay_imports[i].key)) {
            ERR("Duplicate Replay Handler Support for %s\n", 
                replay_imports[i].iminfo.member_name.c_str());
        }
        callid_handlers[replay_imports[i].key] = rep_imfunc;
        unused_callid_handlers.insert(rep_imfunc);
    }

    /* Pre-compute sig indices for replay blocks */
    uint32_t sig_indices[5];
    typelist blocktypes[5] = {
        {WASM_TYPE_I32},
        {WASM_TYPE_I64},
        {WASM_TYPE_F32},
        {WASM_TYPE_F64},
        {}
    };
    for (int i = 0; i < 5; i++) {
        SigDecl s = { .params = {}, .results = blocktypes[i] };
        sig_indices[i] = module.getSigIdx(module.add_sig(s, false));
    }

    /* Generate quick lookup of ignored exported function idxs */
    std::map<FuncDecl*, std::string> func_ignore_map;
    for (const char *func_name: ignored_export_funcnames) {
        if (!set_func_export_map(module, func_name, func_ignore_map)) {
            ERR("Could not find function export: \'%s\'\n", func_name);
        }
    }
    /* Do not instrument the instrument-hooks or start (usually just initialization) */
    func_ignore_map[module.get_start_fn()] = "__#internal#_start_function";
    func_ignore_map[thread_replay_info.lock_func] = "__#internal#_lock_instrument";
    func_ignore_map[thread_replay_info.unlock_func] = "__#internal#_unlock_instrument";

    // Initialize iterator indices
    uint32_t access_tracker = 1;
    uint32_t op_idx = 0;

    std::set<FuncDecl*> remove_importfuncs;

    for (auto &func: module.Funcs()) {
        /* Ignore instrument hooks and exported map functions */
        if (&func == last_instrumented_func) {
            break;
        }
        if (func_ignore_map.count(&func)) {
            continue;
        }
        InstList &insts = func.instructions;
        for (auto institr = insts.begin(); institr != insts.end(); ++institr) {
            InstBasePtr &instruction = *institr;
            bool inc_access_tracker = true;
            InstMetaInfo imeta_info = { 
                .opcode = instruction->getOpcode(),
                .institr = institr,
                .insts = insts,
                .is_import_target = false,
                .target_func = NULL
            };
            switch (instruction->getImmType()) {
                case IMM_FUNC: {
                    std::shared_ptr<ImmFuncInst> call_inst = 
                        static_pointer_cast<ImmFuncInst>(instruction);
                    imeta_info.target_func = call_inst->getFunc();
                    imeta_info.is_import_target = module.isImport(imeta_info.target_func);
                    break;
                }
                case IMM_MEMARG:
                case IMM_MEMARG_LANEIDX:
                case IMM_MEMORY:
                case IMM_DATA_MEMORY:
                case IMM_MEMORYCP: 
                {
                    break;
                }
                default: { inc_access_tracker = false; }
            }

            uint32_t cur_access_tracker = access_tracker;
            if (inc_access_tracker) {
                access_tracker++;
            } else {
                // Ignore non-access instructions
                continue;
            }

            // Executes this only for relevant access instructions
            bool run_replay_replacement = false;
            // Default curr_op for untracked accesses
            ReplayOp curr_op = { 
                .access_idx = cur_access_tracker, .func_idx = 0, 
                .props = nullptr, .num_props = 0 
            };

            bool is_tracked_access_idx = (op_idx < num_ops) && 
                    (cur_access_tracker == ops[op_idx].access_idx);
            if (is_tracked_access_idx) {
                // Tracked: Replay the relevant calls and implicit syncs
                curr_op = ops[op_idx++];
                assert(imeta_info.is_import_target || curr_op.implicit_sync);
                run_replay_replacement = true;
            } else {
                // Untracked: Replay direct import calls only
                run_replay_replacement = imeta_info.is_import_target;
            }

            if (run_replay_replacement) {
                insert_replay_op(module, imeta_info, curr_op, 
                    sig_indices, remove_importfuncs, callid_handlers, 
                    unused_callid_handlers, flags, thread_replay_info,
                    debug_log_call_info);
                TRACE("[%s] Replaced Call Access %d\n", 
                    (is_tracked_access_idx ? "TRACKED" : "UNTRACKED"), cur_access_tracker);
            }
        }
    }
    /* Remove all the replaced import functions */
    for (auto &remfunc : remove_importfuncs) {
        module.remove_func(remfunc);
    }
    /* Remove unused import functions */
    for (auto &unused_func : unused_callid_handlers) {
        FuncDecl *fn = unused_func.func;
        TRACE("Removing unused callid handler: %d\n", module.getFuncIdx(fn));
        module.remove_func(fn);
    }
}
