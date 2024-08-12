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
        bool has_retval, wasm_type_t wasm_ret_type, MemoryDecl *def_mem) {
    InstList addinst;
    // Return value for the arm
    if (has_retval) {
        PUSH_INSTBASE_PTR(get_replay_retval_inst(wasm_ret_type, prop.return_val));
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
static void insert_replay_op(WasmModule &module, FuncDecl &func, InstList::iterator &institr, 
        InstList &insts, ReplayOp &op, uint32_t ret_sig_indices[4], 
        std::set<FuncDecl*> &remove_importfuncs) {
    InstBasePtr &instruction = *institr;
    std::shared_ptr<ImmFuncInst> call_inst = static_pointer_cast<ImmFuncInst>(instruction);
    
    FuncDecl *call_func = call_inst->getFunc();
    uint32_t replay_func_idx = module.getFuncIdx(call_func);
    uint32_t record_func_idx = transform_to_record_func_idx(replay_func_idx);
    SigDecl* call_func_sig = call_func->sig;
    uint32_t call_func_sigidx = module.getSigIdx(call_func->sig);
    // Verify that import function and the indices are consistent with record
    // before proceeding
    assert((module.isImport(call_func) && (op.func_idx == record_func_idx)));

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
    // This is a value for the default case; this should ideally not happen
    if (has_retval) {
        PUSH_INSTBASE_PTR(get_replay_retval_inst(wasm_ret_type, (0xDEADBEEFULL)));
    }
    PUSH_INST (GlobalGetInst(br_tracker));
    PUSH_INST (BrTableInst(labels, op.num_props));
    //PUSH_INST (BrInst(op.num_props));
    PUSH_INST (EndInst());
    for (int i = 0; i < op.num_props; i++) {
        // Assumes only single memory for now
        InstList propinsts = construct_single_replay_prop(
            op.num_props - i - 1, op.props[i], has_retval, wasm_ret_type,
            module.getMemory(0));
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



void r3_replay_instrument (WasmModule &module, void *replay_ops, uint32_t num_ops) {
    ReplayOp *ops = (ReplayOp *)replay_ops;

    printf("In Replay instrument: %d\n", num_ops);
    for (int i = 0; i < num_ops; i++) {
        uint32_t total_stores = 0;
        for (int j = 0; j < ops[i].num_props; j++) {
            total_stores += ops[i].props[j].num_stores;
        }
        printf("Access[%d] | Func Idx: %u | PropOp[%u] | Total Stores: %d\n", 
            ops[i].access_idx, ops[i].func_idx, ops[i].num_props, total_stores);
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
    bool replay_ops_done = false;

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
                    ReplayOp curr_op = ops[op_idx];
                    if (access_tracker == curr_op.access_idx) {
                        insert_replay_op(module, func, institr, insts, curr_op, 
                            sig_indices, remove_importfuncs);
                        printf("Finshed Access %d\n", curr_op.access_idx);
                        op_idx++;
                        if (op_idx == num_ops) {
                            printf("All replay ops done\n");
                            replay_ops_done = true;
                        }
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
            if (replay_ops_done) { break; }
        }
        if (replay_ops_done) { break; }
    }

}