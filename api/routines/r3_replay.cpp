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
  ReplayOpProp* props;
  uint32_t num_props;
  uint64_t max_tid;
} ReplayOp;

typedef struct ReplayImportFuncDecl {
    FuncDecl *func;
    bool debug;

    bool operator<(const struct ReplayImportFuncDecl &other) const {
        return func < other.func;
    }
} ReplayImportFuncDecl;

typedef struct {
    bool is_multithreaded;
    uint64_t num_threads;
    FuncDecl *gettid_func;
    FuncDecl *lock_func;
    FuncDecl *unlock_func;
    std::vector<GlobalDecl*> tid_global_trackers;
} ThreadReplayInfo;

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

// Instructions to enforce synchronization points
#define PUSH_SYNC_TO_ID(id) {  \
    builder.push ({             \
        INST (LoopInst()),  \
        INST (I32ConstInst(id)),  \
        INST (GlobalGetInst(sync_tracker_global)),  \
        INST (I32NeInst()), \
        INST (BrIfInst(0)),  \
        INST (EndInst())  \
    }); \
}

#define PUSH_INC_SYNC_TRACKER(id) { \
    builder.push ({ \
        INST (GlobalGetInst(sync_tracker_global)),  \
        INST (I64ConstInst(1)),  \
        INST (I64AddInst()),  \
        INST (GlobalSetInst(sync_tracker_global))  \
    }); \
}

static InstBuilder construct_single_replay_prop(int br_arm_idx, ReplayOpProp &prop, 
        bool has_retval, wasm_type_t wasm_ret_type, MemoryDecl *def_mem,
        std::map<uint32_t, ReplayImportFuncDecl> &callid_handlers,
        std::set<ReplayImportFuncDecl> &unused_callid_handlers, 
        int64_t flags, ThreadReplayInfo &thread_replay_info) {
    InstBuilder builder = {};
    bool gen_debug_calls = flags;
    // Return value for the arm
    if (has_retval) {
        builder.push_inst (get_replay_retval_inst(wasm_ret_type, prop.return_val));
    }
    // Side-effects for the arm
    bool gen_side_effects = true;
    auto callid_func_it = callid_handlers.find(prop.call_id);
    // For imports that require host calls for side-effects
    if (callid_func_it != callid_handlers.end()) {
        // Generate side effects for debug calls only if flags is set
        gen_side_effects = !(callid_func_it->second.debug) ||
             (callid_func_it->second.debug && gen_debug_calls);
        if (gen_side_effects) {
            unused_callid_handlers.erase(callid_func_it->second);
        }
    }
    if (gen_side_effects) {
        switch(prop.call_id) {
            case RecordInterface::SC_GENERIC: break;
            case RecordInterface::SC_MMAP: {
                uint32_t grow_value = prop.call_args[0];
                if (grow_value > 0) {
                    builder.push ({
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
                builder.push ({
                    INST (I32ConstInst(fd)),
                    INST (I32ConstInst(iov)),
                    INST (I32ConstInst(iovcnt)),
                    INST (CallInst(callid_func_it->second.func))
                });
                break;
            }
            case RecordInterface::SC_PROC_EXIT: 
            case RecordInterface::SC_THREAD_EXIT: {
                int32_t status = prop.call_args[0];
                builder.push ({
                    INST (I32ConstInst(status)),
                    INST (CallInst(callid_func_it->second.func))
                });
                break;
            }
            case RecordInterface::SC_THREAD_SPAWN: {
                int32_t setup_fn_ptr = prop.call_args[0];
                int32_t thread_args_ptr = prop.call_args[1];
                builder.push ({
                    INST (I32ConstInst(setup_fn_ptr)),
                    INST (I32ConstInst(thread_args_ptr)),
                    INST (CallInst(callid_func_it->second.func))
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
                        builder.push ({
                            //I32ConstInst(addr),
                            //I32ConstInst(val),
                            //I64ConstInst(-1),
                            INST (I32ConstInst(addr)),
                            INST (I32ConstInst(futex_op)),
                            INST (I32ConstInst(val)),
                            INST (CallInst(callid_func_it->second.func))
                            //MemoryAtomicWait32Inst(2, 0, def_mem),
                            //DropInst()
                        });
                        break;
                    }
                    case RecordInterface::FutexWake: {
                        builder.push ({
                            //I32ConstInst(addr),
                            //I32ConstInst(val),
                            INST (I32ConstInst(addr)),
                            INST (I32ConstInst(futex_op)),
                            INST (I32ConstInst(val)),
                            INST (CallInst(callid_func_it->second.func))
                            //MemoryAtomicNotifyInst(2, 0, def_mem),
                            //DropInst()
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
        builder.push ({
            INST (I32ConstInst(store.addr)),
            INST (I64ConstInst(store.value))
        });
        switch (store.size) {
            case 1: builder.push_inst (I64Store8Inst(0, 0, def_mem)); break;
            case 2: builder.push_inst (I64Store16Inst(0, 0, def_mem)); break;
            case 4: builder.push_inst (I64Store32Inst(0, 0, def_mem)); break;
            case 8: builder.push_inst (I64StoreInst(0, 0, def_mem)); break;
            default: { ERR("R3-Replay-Error: Unsupported size %u", store.size); }
        }
    }
    builder.push_inst (BrInst(br_arm_idx));
    return builder;
}


/* Inserts the return value and store replay operations (for all threads)
   Removes the call to the respective function
   */
static void insert_replay_op(WasmModule &module, FuncDecl *call_func, 
        InstList::iterator &institr, InstList &insts, 
        ReplayOp &op, uint32_t ret_sig_indices[4], 
        std::set<FuncDecl*> &remove_importfuncs, 
        std::map<uint32_t, ReplayImportFuncDecl> &callid_handlers,
        std::set<ReplayImportFuncDecl> &unused_callid_handlers, 
        int64_t flags, ThreadReplayInfo &thread_replay_info) {

    SigDecl* call_func_sig = call_func->sig;
    uint32_t call_func_sigidx = module.getSigIdx(call_func->sig);

    bool empty_replay_op = (op.num_props == 0);
    // Verify that import function and the indices are consistent with record
    // before proceeding
    {
        if (!empty_replay_op) {
            uint32_t replay_func_idx = module.getFuncIdx(call_func);
            uint32_t record_func_idx = transform_to_record_func_idx(replay_func_idx);
            assert(op.func_idx == record_func_idx);
        }
    }

    static GlobalDecl br_tracker_decl = {
        .type = WASM_TYPE_I32, .is_mutable = true, 
        .init_expr_bytes = INIT_EXPR (I32_CONST, 0)
    };

    // Make sure the function only has max one return value
    assert (call_func_sig->results.size() <= 1);
    // If non-void, ret_sigidx takes the value for the block
    int64_t ret_sigidx;
    wasm_type_t wasm_ret_type;
    bool has_retval = true;
    if (call_func_sig->results.size() == 1) {
        wasm_ret_type = call_func_sig->results.front();
        ret_sigidx = ret_sig_indices[to_wasmret(wasm_ret_type) - RET_I32];
    } else {
        has_retval = false;
        ret_sigidx = ret_sig_indices[4];
    }


    InstBuilder builder = {};

    if (empty_replay_op) {
        // If no props, skip replay function construction and insert a nop
        if (has_retval) {
            builder.push_inst (get_replay_retval_inst(wasm_ret_type, 0));
        } 
        builder.push_inst (EndInst());
    }
    else {
        // Replay function instruction construction
        builder.push({
            INST (CallInst(thread_replay_info.gettid_func)),
            INST (DropInst())
        });

        //std::list<uint32_t> thread_labels(op.max_tid);
        //std::iota(thread_labels.begin(), thread_labels.end(), -1);
        //thread_labels.front() = 0;

        ///* New insertions */
        //// Thread Replay Info: Starts from 0 since TID starts with 1
        //for (int i = 0; i < op.max_tid; i++) {
        //    PUSH_INST (BlockInst(ret_sigidx));
        //}
        //// This is a value for the default case; this should never be used by valid replays
        //if (has_retval) {
        //    PUSH_INSTBASE_PTR(get_replay_retval_inst(wasm_ret_type, (0xDEADBEEFULL)));
        //}
        //// Switch table on thread id
        //PUSH_INST (CallInst(thread_replay_info.gettid_func));
        //PUSH_INST (I32ConstInst(1));
        //PUSH_INST (I32SubInst());
        //PUSH_INST (BrTableInst(thread_labels, op.max_tid));
        //PUSH_INST (EndInst());
        /** */

        // Lock the replay operation
        if (thread_replay_info.is_multithreaded) {
            builder.push_inst (CallInst(thread_replay_info.lock_func));
        }
        // Start of replay unit
        builder.push_inst (BlockInst(ret_sigidx));

        // Insert global trackers
        GlobalDecl* br_tracker = module.add_global(br_tracker_decl);
        for (int i = 0; i < op.num_props; i++) {
            builder.push_inst (BlockInst(ret_sigidx));
        }
        std::list<uint32_t> seq_labels(op.num_props);
        std::iota(seq_labels.begin(), seq_labels.end(), 0);
        // Insert switch table on the branch tracker
        builder.push_inst (BlockInst(ret_sigidx));
        // This is a value for the default case; this should never be used by valid replays
        if (has_retval) {
            builder.push_inst (get_replay_retval_inst(wasm_ret_type, (0xDEADBEEFULL)));
        }
        builder.push_inst (GlobalGetInst(br_tracker));
        // Default skips global variable increment
        builder.push({
            INST (BrTableInst(seq_labels, op.num_props + 1)),
            INST (EndInst())
        });
        for (int i = 0; i < op.num_props; i++) {
            // Assumes only single memory for now
            InstBuilder prop_builder = construct_single_replay_prop(
                op.num_props - i - 1, op.props[i], has_retval, wasm_ret_type,
                module.getMemory(0), callid_handlers, unused_callid_handlers, 
                flags, thread_replay_info);
            builder.splice(prop_builder);
            builder.push_inst (EndInst());
        }

        // End of replay unit
        builder.push({
            INST (GlobalGetInst(br_tracker)),
            INST (I32ConstInst(1)),
            INST (I32AddInst()),
            INST (GlobalSetInst(br_tracker)),
            INST (EndInst())
        });
        // Unlock the replay operation
        if (thread_replay_info.is_multithreaded) {
            builder.push_inst (CallInst(thread_replay_info.unlock_func));
        }
        // End of function
        builder.push_inst (EndInst());
    }

    /* Insert replay instructions into module, remove import call, and reset iterator */
    // Setup the replay op function declaration
    char replay_func_debug_name[100];
    ImportDecl *import_decl = module.get_import_name_from_func(call_func);
    sprintf(replay_func_debug_name, "__replay_instrument_%s_%s", 
        import_decl->mod_name.c_str(), 
        import_decl->member_name.c_str());
    FuncDecl replay_ops_func_decl = {
        .sig = call_func_sig,
        .pure_locals = wasm_localcsv_t(),
        .num_pure_locals = 0,
        .instructions = builder.get_buf()
    };
    FuncDecl* replay_ops_func = module.add_func(replay_ops_func_decl, NULL, replay_func_debug_name);
    InstList &replay_func_insts = replay_ops_func->instructions;

    // Remove import call
    InstItr next_inst = std::next(institr);
    insts.erase(institr);
    insts.insert(next_inst, INST(CallInst(replay_ops_func)));
    institr = std::prev(next_inst);
    remove_importfuncs.insert(call_func);
}



void r3_replay_instrument (WasmModule &module, void *replay_ops, 
        uint32_t num_ops, int64_t flags) {
    ReplayOp *ops = (ReplayOp *)replay_ops;

    /* We actually don't use this added page at all (for now)
    However, we need to add it purely to maintain address space consistency
    with the recorded module which added a page for instrumentation */
    MemoryDecl *def_mem = module.getMemory(0);
    FuncDecl *mutex_funcs[2];
    uint32_t mutex_addr = (add_pages(def_mem, 1) * WASM_PAGE_SIZE);
    FuncDecl *last_preinstrumented_func = &module.Funcs().back();

    /* Create all thread-related management */
    create_mutex_funcs(module, mutex_addr, mutex_funcs);
    ThreadReplayInfo thread_replay_info = {
        .lock_func = mutex_funcs[0],
        .unlock_func = mutex_funcs[1],
    };
    // Global to enforce happens-before synchronization and thread values
    for (int i = 0; i < num_ops; i++) {
        while (thread_replay_info.num_threads < ops[i].max_tid) {
            GlobalDecl gdecl = {
                .type = WASM_TYPE_I64, .is_mutable = true,
                .init_expr_bytes = INIT_EXPR (I64_CONST, 0)
            };
            thread_replay_info.tid_global_trackers.push_back(
                module.add_global(gdecl)
            );
            thread_replay_info.num_threads++;
        }
    }
    thread_replay_info.is_multithreaded = (thread_replay_info.num_threads > 1);

    /* Engine callbacks for specialized replay handlers */
    std::map<uint32_t, ReplayImportFuncDecl> callid_handlers;
    std::set<ReplayImportFuncDecl> unused_callid_handlers;
    for (int i = 0; i < NUM_REPLAY_IMPORTS; i++) {
        ReplayImportFuncDecl rep_imfunc = { 
            .func = add_r3_import_function(module, replay_imports[i]), 
            .debug = replay_imports[i].debug 
        };
        if (replay_imports[i].key != 0) {
            callid_handlers[replay_imports[i].key] = rep_imfunc;
            unused_callid_handlers.insert(rep_imfunc);
        } else {
            if (replay_imports[i].iminfo.member_name == "SC_gettid") {
                // Set the tid import function
                thread_replay_info.gettid_func = rep_imfunc.func;
            }
        }
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

    // Initialize iterator indices
    uint32_t access_tracker = 1;
    uint32_t op_idx = 0;

    std::set<FuncDecl*> remove_importfuncs;

    for (auto &func: module.Funcs()) {
        /* Ignore instrument hooks and exported map functions */
        if (&func == last_preinstrumented_func) {
            break;
        }
        if (func_ignore_map.count(&func)) {
            continue;
        }
        InstList &insts = func.instructions;
        for (auto institr = insts.begin(); institr != insts.end(); ++institr) {
            InstBasePtr &instruction = *institr;
            bool inc_access_tracker = true;
            switch (instruction->getImmType()) {
                // Fallthrough here to increment access for this type as well
                case IMM_FUNC: {
                    std::shared_ptr<ImmFuncInst> call_inst = 
                        static_pointer_cast<ImmFuncInst>(instruction);
                    FuncDecl *call_func = call_inst->getFunc();
                    // Reinstrument all import calls to replay
                    if (module.isImport(call_func)) {
                        bool tracked_import = (op_idx < num_ops) && 
                            (access_tracker == ops[op_idx].access_idx);
                        // Imports with no replay data use the empty replay-op
                        ReplayOp curr_op = (tracked_import ? 
                            ops[op_idx++] :
                            (ReplayOp) { .access_idx = 0, .func_idx = 0, .props = nullptr, .num_props = 0 }
                        );
                        // Mutex insertion around replay operation
                        insert_replay_op(module, call_func, institr, insts, curr_op, 
                            sig_indices, remove_importfuncs, callid_handlers, 
                            unused_callid_handlers, flags, thread_replay_info);
                        TRACE("[%s] Replaced Call Access %d\n", 
                            (tracked_import ? "TRACKED" : "UNTRACKED"), access_tracker);
                    }
                }
                case IMM_MEMARG:
                case IMM_MEMARG_LANEIDX:
                case IMM_MEMORY:
                case IMM_DATA_MEMORY:
                case IMM_MEMORYCP: {
                    break;
                }
                default: { inc_access_tracker = false; }
            }
            if (inc_access_tracker) {
                access_tracker++;
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
