#include "routine_common.h"
#include "r3_common.h"
#include <set>
#include <cassert>
#include <cstring>
#include <numeric>

#define INST(inv) InstBasePtr(new inv)
#define PUSH_INST(inv) addinst.push_back(InstBasePtr(new inv))
#define PUSH_INSTBASE_PTR(inv) addinst.push_back(inv)


/* Replay Op C Structure */
typedef struct {
    int32_t addr;
    uint32_t size;
    int64_t value;
} ReplayMemStore;

typedef struct {
    int64_t return_val;
    uint32_t call_id;
    int64_t call_args[3];
    ReplayMemStore* stores;
    uint32_t num_stores;
} ReplayOpProp;

typedef struct {
  uint32_t access_idx;
  uint32_t func_idx;
  ReplayOpProp* props;
  uint32_t num_props;
} ReplayOp;

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

static InstList construct_single_replay_prop(int br_arm_idx, ReplayOpProp &prop, 
        bool has_retval, wasm_type_t wasm_ret_type, MemoryDecl *def_mem,
        std::map<uint32_t, ReplayImportFuncDecl> &callid_handlers,
        std::set<ReplayImportFuncDecl> &unused_callid_handlers, 
        int64_t flags) {
    InstList addinst;
    bool gen_debug_calls = flags;
    // Return value for the arm
    if (has_retval) {
        PUSH_INSTBASE_PTR(get_replay_retval_inst(wasm_ret_type, prop.return_val));
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
            case SC_GENERIC: break;
            case SC_MMAP: {
                uint32_t grow_value = prop.call_args[0];
                if (grow_value > 0) {
                    PUSH_INST (I32ConstInst(grow_value));
                    PUSH_INST (MemoryGrowInst(def_mem));
                    PUSH_INST (DropInst());
                }
                break;
            }
            case SC_WRITEV: {
                int32_t fd = prop.call_args[0];
                int32_t iov = prop.call_args[1];
                uint32_t iovcnt = prop.call_args[2];
                PUSH_INST (I32ConstInst(fd));
                PUSH_INST (I32ConstInst(iov));
                PUSH_INST (I32ConstInst(iovcnt));
                PUSH_INST (CallInst(callid_func_it->second.func));
                break;
            }
            case SC_PROC_EXIT: {
                int32_t status = prop.call_args[0];
                PUSH_INST (I32ConstInst(status));
                PUSH_INST (CallInst(callid_func_it->second.func));
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
        PUSH_INST (I32ConstInst(store.addr));
        PUSH_INST (I64ConstInst(store.value));
        switch (store.size) {
            case 1: PUSH_INST(I64Store8Inst(0, 0, def_mem)); break;
            case 2: PUSH_INST(I64Store16Inst(0, 0, def_mem)); break;
            case 4: PUSH_INST(I64Store32Inst(0, 0, def_mem)); break;
            case 8: PUSH_INST(I64StoreInst(0, 0, def_mem)); break;
            default: { ERR("R3-Replay-Error: Unsupported size %u", store.size); }
        }
    }
    PUSH_INST (BrInst(br_arm_idx));
    return addinst;
}


/* Inserts the return value and store replay operation 
   Removes the call to the respective function
   */
static void insert_replay_op(WasmModule &module, FuncDecl *call_func, 
        InstList::iterator &institr, InstList &insts, 
        ReplayOp &op, uint32_t ret_sig_indices[4], 
        std::set<FuncDecl*> &remove_importfuncs, 
        std::map<uint32_t, ReplayImportFuncDecl> &callid_handlers,
        std::set<ReplayImportFuncDecl> &unused_callid_handlers, 
        int64_t flags) {
    SigDecl* call_func_sig = call_func->sig;
    uint32_t call_func_sigidx = module.getSigIdx(call_func->sig);
    // Verify that import function and the indices are consistent with record
    // before proceeding
    {
        if (op.num_props != 0) {
            uint32_t replay_func_idx = module.getFuncIdx(call_func);
            uint32_t record_func_idx = transform_to_record_func_idx(replay_func_idx);
            assert(op.func_idx == record_func_idx);
        }
    }

    static GlobalDecl br_tracker_decl = {
        .type = WASM_TYPE_I32, .is_mutable = true, 
        .init_expr_bytes = INIT_EXPR (I32_CONST, 0)
    };

    InstList addinst;
    // Start of replay unit
    PUSH_INST (BlockInst(call_func_sigidx));
    for (int i = 0; i < call_func_sig->params.size(); i++) {
        PUSH_INST (DropInst());
    }

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

    // Insert global branch tracker
    GlobalDecl* br_tracker = module.add_global(br_tracker_decl);
    for (int i = 0; i < op.num_props; i++) {
        PUSH_INST (BlockInst(ret_sigidx));
    }
    std::list<uint32_t> labels(op.num_props);
    std::iota(labels.begin(), labels.end(), 0);
    // Insert switch table on the branch tracker
    PUSH_INST (BlockInst(ret_sigidx));
    // This is a value for the default case; this should never be used by valid replays
    if (has_retval) {
        PUSH_INSTBASE_PTR(get_replay_retval_inst(wasm_ret_type, (0xDEADBEEFULL)));
    }
    PUSH_INST (GlobalGetInst(br_tracker));
    // Default skips global variable increment
    PUSH_INST (BrTableInst(labels, op.num_props + 1));
    PUSH_INST (EndInst());
    for (int i = 0; i < op.num_props; i++) {
        // Assumes only single memory for now
        InstList propinsts = construct_single_replay_prop(
            op.num_props - i - 1, op.props[i], has_retval, wasm_ret_type,
            module.getMemory(0), callid_handlers, unused_callid_handlers, flags);
        addinst.insert(addinst.end(), propinsts.begin(), propinsts.end());
        PUSH_INST (EndInst());
    }

    // End of replay unit
    PUSH_INST (GlobalGetInst(br_tracker));
    PUSH_INST (I32ConstInst(1));
    PUSH_INST (I32AddInst());
    PUSH_INST (GlobalSetInst(br_tracker));
    PUSH_INST (EndInst());

    // Insert instructions, remove import call, and reset iterator
    InstItr next_inst = std::next(institr);
    insts.erase(institr);
    insts.insert(next_inst, addinst.begin(), addinst.end());
    institr = std::prev(next_inst);
    remove_importfuncs.insert(call_func);
}



void r3_replay_instrument (WasmModule &module, void *replay_ops, 
        uint32_t num_ops, int64_t flags) {
    ReplayOp *ops = (ReplayOp *)replay_ops;

    printf("FLAGS: %ld\n", flags);
    /* We actually don't use this added page at all (for now)
    However, we need to add it purely to maintain address space consistency
    with the recorded module which added a page for instrumentation */
    MemoryDecl *def_mem = module.getMemory(0);
    uint32_t old_init_pages = add_pages(def_mem, 1);

    /* Engine callbacks for specialized replay handlers */
    std::map<uint32_t, ReplayImportFuncDecl> callid_handlers;
    std::set<ReplayImportFuncDecl> unused_callid_handlers;
    for (int i = 0; i < NUM_REPLAY_IMPORTS; i++) {
        ReplayImportFuncDecl rep_imfunc = { 
            .func = add_r3_import_function(module, replay_imports[i]), 
            .debug = replay_imports[i].debug 
        };
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
    std::map<FuncDecl*, std::string> func_export_map;
    for (const char *func_name: ignored_export_funcnames) {
        if (!set_func_export_map(module, func_name, func_export_map)) {
            ERR("Could not find function export: \'%s\'\n", func_name);
        }
    }

    // Initialize iterator indices
    uint32_t access_tracker = 1;
    uint32_t op_idx = 0;

    std::set<FuncDecl*> remove_importfuncs;

    for (auto &func: module.Funcs()) {
        /* Ignore instrument hook and exported map functions */
        if (func_export_map.count(&func)) {
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
                        // Untracked imports with no replay data use the empty replay-op
                        ReplayOp curr_op = (tracked_import ? 
                            ops[op_idx++] :
                            (ReplayOp) { .access_idx = 0, .func_idx = 0, .props = nullptr, .num_props = 0 }
                        );
                        insert_replay_op(module, call_func, institr, insts, curr_op, 
                            sig_indices, remove_importfuncs, callid_handlers, 
                            unused_callid_handlers, flags);
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