#include "routine_common.h"
#include "r3_common.h"
#include <cassert>

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

void insert_replay_op(WasmModule &module, InstList::iterator &institr, ReplayOp &op) {
    InstBasePtr &instruction = *institr;
    std::shared_ptr<ImmFuncInst> call_inst = static_pointer_cast<ImmFuncInst>(instruction);
    FuncDecl *call_func = call_inst->getFunc();
    uint32_t replay_func_idx = module.getFuncIdx(call_func);
    uint32_t record_func_idx = transform_to_record_func_idx(replay_func_idx);
    // Verify that import function and the indices are consistent with record
    // before proceeding
    char buffer[100];
    assert((module.isImport(call_func) && (op.func_idx == record_func_idx)));
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

    /* Generate quick lookup of ignored exported function idxs */
    std::map<FuncDecl*, std::string> func_export_map;
    for (const char *func_name: ignored_export_funcs) {
        if (!set_func_export_map(module, func_name, func_export_map)) {
        ERR("Could not find function export: \'%s\'\n", func_name);
        }
    }

    // Initialize iterator indices
    uint32_t access_tracker = 1;
    uint32_t op_idx = 0;
    bool replay_ops_done = false;

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
                        insert_replay_op(module, institr, curr_op);
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