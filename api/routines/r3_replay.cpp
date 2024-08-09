#include "routine_common.h"
#include "r3_common.h"



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
}