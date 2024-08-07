#include "routine_common.h"
#include "r3_memops_table.h"
#include <cstring>

typedef enum {
  RET_PH = 0,
  RET_VOID,
  RET_I32,
  RET_I64,
  RET_F32,
  RET_F64
} WasmRet;

typedef struct {
  WasmRet type;
  uint32_t local;
} RetVal;

/* Types of Call instructions */
typedef enum {
  SC_UNKNOWN = 0,
  /* Specialized Calls */
  SC_MMAP,
  SC_WRITEV,
  /* Lockless Calls */
  //SC_THREAD_SPAWN,
  //SC_FUTEX,
  //SC_EXIT,
  /* All other generic imports */
  SC_GENERIC
} CallID;

typedef struct {
  RetVal ret;
  MemopProp prop;
  uint32_t offset;
  uint32_t base_addr_local;
  uint32_t accwidth_local; // For prop.size = -1, the width is dynamically stored in this local
  uint16_t opcode; // Opcode is kept for size=0, which handles special cases
} MemopInstInfo;

struct CallInstInfo {
  RetVal ret;
  uint16_t opcode;
  FuncDecl *target;
  CallID call_id;
};

union RecordInstInfo {
  enum { MEMOP, CALL } type;
  MemopInstInfo Memop;
  CallInstInfo Call;
};


/* Instruction-generation macros */
#define PUSH_INST(inv)      addinst.push_back(InstBasePtr(new inv));
#define PUSH_INST_PTR(inv)  addinst.push_back(InstBasePtr(inv)); 

#define BLOCK_INST(v) {     \
  insblock = true;          \
  PUSH_INST (BlockInst(v)); \
}

#define SAVE_TOP(var) {          \
  PUSH_INST (LocalSetInst(var)); \
}
#define RESTORE_TOP(var) {       \
  PUSH_INST (LocalGetInst(var)); \
} 
#define TEE_TOP(var) {           \
  PUSH_INST (LocalTeeInst(var)); \
}

#define SETUP_INIT_COMMON(_insttype)                                                 \
  InstList addinst;                                                                  \
  InstBasePtr instruction = (*itr);                                                  \
  bool insblock = false;                                                             \
  WasmRet stackrets = RET_PH;                                                        \
  uint16_t opcode = instruction->getOpcode();                                        \
  uint32_t local_f64 = local_idxs[0];                                                \
  uint32_t local_f32 = local_idxs[1];                                                \
  uint32_t local_i64_1 = local_idxs[2];                                              \
  uint32_t local_i64_2 = local_idxs[3];                                              \
  uint32_t local_i32_1 = local_idxs[4];                                              \
  uint32_t local_i32_2 = local_idxs[5];                                              \
  uint32_t local_i32_3 = local_idxs[6];                                              \
                                                                                     \
  uint32_t local_i32_ret = local_idxs[7];                                            \
  uint32_t local_i64_ret = local_idxs[8];                                            \
  uint32_t local_f32_ret = local_idxs[9];                                            \
  uint32_t local_f64_ret = local_idxs[10];                                           \
                                                                                     \
  uint32_t sig_i32 = sig_idxs[0];                                                    \
  uint32_t sig_i32_2 = sig_idxs[1];                                                  \
  uint32_t sig_i32_i64 = sig_idxs[2];                                                \
  uint32_t sig_i32_f32 = sig_idxs[3];                                                \
  uint32_t sig_i32_f64 = sig_idxs[4];                                                \
  uint32_t sig_i32_3 = sig_idxs[5];                                                  \
  uint32_t sig_i32_i64_2 = sig_idxs[6];


#define LOCAL_RET_SET_COMMON() \
    switch (ret.type) {                                                                        \
      case RET_I32: ret.local = local_idxs[7]; PUSH_INST (LocalSetInst(local_i32_ret)); break; \
      case RET_I64: ret.local = local_idxs[8]; PUSH_INST (LocalSetInst(local_i64_ret)); break; \
      case RET_F32: ret.local = local_idxs[9]; PUSH_INST (LocalSetInst(local_f32_ret)); break; \
      case RET_F64: ret.local = local_idxs[10]; PUSH_INST (LocalSetInst(local_f64_ret)); break; \
      default: {}                                                                              \
    }


#define MEMOP_SETUP_INIT(_insttype)                        \
  SETUP_INIT_COMMON(_insttype)                             \
  /* Only used for some operations that determine width */ \
  uint32_t local_addr = local_i32_3;                       \
  uint32_t local_accwidth = -1;                            \
  uint32_t mem_offset = 0;                                 \
  std::shared_ptr<_insttype> mem_inst = static_pointer_cast<_insttype>(instruction); 

#define MEMOP_SETUP_END(_insttype, updater)                                                    \
  RetVal ret = { .type = stackrets, .local = (uint32_t)-1 };                                   \
  if (insblock) {                                                                              \
    _insttype *new_meminst = new _insttype(*mem_inst);                                         \
    { updater; }                                                                               \
    PUSH_INST_PTR (new_meminst);                                                               \
    LOCAL_RET_SET_COMMON(); \
    PUSH_INST (EndInst());                                                                     \
  }                                                                                            \
  recinfo = { .ret = ret, .prop = memop_inst_table[opcode],                                    \
    .offset = mem_offset, .base_addr_local = local_addr,                                       \
    .accwidth_local = local_accwidth, .opcode = opcode };


#define CALL_SETUP_INIT(_insttype)       \
  SETUP_INIT_COMMON(_insttype)           \
  uint32_t local_i32_4 = local_idxs[11]; \
  uint32_t local_i64_3 = local_idxs[12]; \
  uint32_t local_i32_5 = local_idxs[15]; \
  std::shared_ptr<_insttype> call_inst = static_pointer_cast<_insttype>(instruction);

// Local rets are ordered by RET_I32, RET_I64, RET_F32, RET_F64
#define CALL_SETUP_END(_insttype, updater)  \
  RetVal ret = { .type = stackrets, .local = local_idxs[7 + ret.type - RET_I32] };                                   \
  recinfo = { .ret = ret, .opcode = opcode, .target = target, .call_id = call_id };
    
//.mod_name = mod_name, .member_name = member_name, .args = args };

#define DEFAULT_ERR_CASE() {  \
  ERR("R3-Record-Error: Cannot support opcode %04X (%s)\n", opcode, opcode_table[opcode].mnemonic); \
}

#define SET_RET(r) {                                   \
  stackrets = ((stackrets == RET_PH) ? r : stackrets); \
}


/* Add pages of memory statically. Return the start page */
uint32_t add_pages(WasmModule &module, uint32_t num_pages) {
  wasm_limits_t &memlimit = module.getMemory(0)->limits;
  uint32_t memdata_end = memlimit.initial * WASM_PAGE_SIZE;

  uint32_t retval = memlimit.initial;
  memlimit.initial += num_pages;
  if (memlimit.has_max && (memlimit.initial > memlimit.max)) {
    throw std::runtime_error("Not enough memory to instrument");
  }
  return retval;
}


/* Mutex Lock/Unlock function creation */
void create_helper_funcs(WasmModule &module, uint32_t mutex_addr, FuncDecl *funcs[2]) {
  MemoryDecl *memory = module.getMemory(0);
  SigDecl sig = {.params= {}, .results = {}};
  SigDecl *void_sig = module.add_sig(sig, false);
  uint32_t void_sig_idx = module.getSigIdx(void_sig);

#define INST(v) InstBasePtr(new v)
  FuncDecl fn1 = {
    .sig = void_sig,
    .pure_locals = wasm_localcsv_t(),
    .num_pure_locals = 0,
    .instructions = {
      INST (BlockInst(void_sig_idx)),
      INST (LoopInst(void_sig_idx)),
      // Try lock
      INST (I32ConstInst(mutex_addr)),
      INST (I32ConstInst(0)),
      INST (I32ConstInst(1)),
      INST (I32AtomicRmwCmpxchgInst(2, 0, memory)),
      //
      INST (I32EqzInst()),
      INST (BrIfInst(1)),
      // Wait for notify
      INST (I32ConstInst(mutex_addr)),
      INST (I32ConstInst(1)),
      INST (I64ConstInst(-1)),
      INST (MemoryAtomicWait32Inst(2, 0, memory)),
      //
      INST (DropInst()),
      INST (BrInst(0)),

      INST (EndInst()),
      INST (EndInst()),
      INST (EndInst())
    }
  };

  FuncDecl fn2 = {
    .sig = void_sig,
    .pure_locals = wasm_localcsv_t(),
    .num_pure_locals = 0,
    .instructions = {
      // Unlock
      INST (I32ConstInst(mutex_addr)),
      INST (I32ConstInst(0)),
      INST (I32AtomicStoreInst(2, 0, memory)),
      // Notify 1 thread max
      INST (I32ConstInst(mutex_addr)),
      INST (I32ConstInst(1)),
      INST (MemoryAtomicNotifyInst(2, 0, memory)),
      //
      INST (DropInst()),
      INST (EndInst())
    }
  };
#undef INST

  funcs[0] = module.add_func(fn1, NULL, "lock_instrument");
  funcs[1] = module.add_func(fn2, NULL, "unlock_instrument");
}


static void set_func_export_map(WasmModule &module, std::string name, std::map<FuncDecl*, std::string>& export_map) {
  ExportDecl *exp = module.find_export(name);
  if (exp && exp->kind == KIND_FUNC) {
    export_map[exp->desc.func] = name;
    return;
  }
  ERR("Could not find function export: \'%s\'\n", name.c_str());
}


InstList setup_memarg_laneidx_record_instrument (std::list<InstBasePtr>::iterator &itr, 
    uint32_t local_idxs[11], uint32_t sig_idxs[7], MemoryDecl *record_mem,
    uint32_t access_idx, MemopInstInfo &recinfo) {

  MEMOP_SETUP_INIT(ImmMemargLaneidxInst);

  switch (opcode) {
    /*** ImmMemargLaneidx type ***/
    default: DEFAULT_ERR_CASE()
  }

  MEMOP_SETUP_END(ImmMemargLaneidxInst,
    new_meminst->setMemory(record_mem)
  );

  return addinst;
}


InstList setup_memory_record_instrument (std::list<InstBasePtr>::iterator &itr, 
    uint32_t local_idxs[11], uint32_t sig_idxs[7], MemoryDecl *record_mem, 
    uint32_t access_idx, MemopInstInfo &recinfo) {

  MEMOP_SETUP_INIT(ImmMemoryInst);

  switch (opcode) {
    /*** ImmMemory type ***/
    /* I32 | I32 | I32 */
    case WASM_OP_MEMORY_FILL: {
                                 local_accwidth = local_i32_1;
                              }
                              {
                                 BLOCK_INST(sig_i32_3);
                                 SAVE_TOP (local_i32_1);
                                 SAVE_TOP (local_i32_2);
                                 TEE_TOP (local_addr);
                                 RESTORE_TOP (local_i32_2);
                                 RESTORE_TOP (local_i32_1);

                                 RESTORE_TOP (local_addr);
                                 RESTORE_TOP (local_i32_2);
                                 RESTORE_TOP (local_i32_1);
                                 break;
                              }
    /* */
    case WASM_OP_MEMORY_SIZE: {
                                 SET_RET (RET_I32);
                              }
                              {
                                 BLOCK_INST (-0x40);
                                 break;
                              }
    /* I32 */
    case WASM_OP_MEMORY_GROW: {
                                SET_RET (RET_I32);
                              }
                              {
                                BLOCK_INST(sig_i32);
                                TEE_TOP(local_addr);

                                RESTORE_TOP(local_addr);
                                break;
                              }
    default: DEFAULT_ERR_CASE()
  }

  MEMOP_SETUP_END(ImmMemoryInst,
    new_meminst->setMemory(record_mem)
  );

  return addinst;
}


InstList setup_data_memory_record_instrument (std::list<InstBasePtr>::iterator &itr, 
    uint32_t local_idxs[11], uint32_t sig_idxs[7], MemoryDecl *record_mem, 
    uint32_t access_idx, MemopInstInfo &recinfo) {

  MEMOP_SETUP_INIT(ImmDataMemoryInst);

  switch (opcode) {
    /*** ImmDataMemory type ***/
    /* I32 | I32 | I32 */
    case WASM_OP_MEMORY_INIT: {
                                 BLOCK_INST(sig_i32_3);
                                 SAVE_TOP (local_i32_1);
                                 SAVE_TOP (local_i32_2);
                                 TEE_TOP (local_addr);
                                 RESTORE_TOP (local_i32_2);
                                 RESTORE_TOP (local_i32_1);

                                 RESTORE_TOP (local_addr);
                                 RESTORE_TOP (local_i32_2);
                                 RESTORE_TOP (local_i32_1);
                                 break;
                              }
    default: DEFAULT_ERR_CASE()
  }

  local_accwidth = local_i32_1;

  MEMOP_SETUP_END(ImmDataMemoryInst,
    new_meminst->setMemory(record_mem)
  );

  return addinst;
}


InstList setup_memorycp_record_instrument (std::list<InstBasePtr>::iterator &itr, 
    uint32_t local_idxs[11], uint32_t sig_idxs[7], MemoryDecl *record_mem, 
    uint32_t access_idx, MemopInstInfo &recinfo) {

  MEMOP_SETUP_INIT(ImmMemorycpInst);

  switch (opcode) {
    /*** ImmMemorycp type ***/
    /* I32 | I32 | I32 */
    case WASM_OP_MEMORY_COPY: {
                                 BLOCK_INST(sig_i32_3);
                                 SAVE_TOP (local_i32_1);
                                 SAVE_TOP (local_i32_2);
                                 TEE_TOP (local_addr);
                                 RESTORE_TOP (local_i32_2);
                                 RESTORE_TOP (local_i32_1);

                                 RESTORE_TOP (local_addr);
                                 RESTORE_TOP (local_i32_2);
                                 RESTORE_TOP (local_i32_1);
                                 break;
                              }
    default: DEFAULT_ERR_CASE()
  }

  local_accwidth = local_i32_1;

  MEMOP_SETUP_END(ImmMemorycpInst,
    new_meminst->setDstMemory(record_mem)
  );

  return addinst;
}


InstList setup_memarg_record_instrument (std::list<InstBasePtr>::iterator &itr, 
    uint32_t local_idxs[11], uint32_t sig_idxs[7], MemoryDecl *record_mem, 
    uint32_t access_idx, MemopInstInfo &recinfo) {

  MEMOP_SETUP_INIT(ImmMemargInst);

  switch (opcode) {
    /*** ImmMemarg type ***/
    /* I32 */
    case WASM_OP_I32_LOAD:
    case WASM_OP_I32_ATOMIC_LOAD:
    case WASM_OP_I32_LOAD8_S:
    case WASM_OP_I32_LOAD8_U: 
    case WASM_OP_I32_ATOMIC_LOAD8_U:
    case WASM_OP_I32_LOAD16_S:
    case WASM_OP_I32_LOAD16_U:
    case WASM_OP_I32_ATOMIC_LOAD16_U: {
                                        SET_RET (RET_I32);
                                      }

    case WASM_OP_I64_LOAD:
    case WASM_OP_I64_LOAD8_S:
    case WASM_OP_I64_LOAD8_U:
    case WASM_OP_I64_LOAD16_S:
    case WASM_OP_I64_LOAD16_U:
    case WASM_OP_I64_LOAD32_S:
    case WASM_OP_I64_LOAD32_U:
    case WASM_OP_I64_ATOMIC_LOAD:
    case WASM_OP_I64_ATOMIC_LOAD8_U:
    case WASM_OP_I64_ATOMIC_LOAD16_U:
    case WASM_OP_I64_ATOMIC_LOAD32_U: {
                                        SET_RET (RET_I64);
                                      }

    case WASM_OP_F32_LOAD: {
                             SET_RET (RET_F32);
                           }
    case WASM_OP_F64_LOAD: {
                             SET_RET (RET_F64);
                           }
                           {
                              BLOCK_INST(sig_i32);
                              TEE_TOP(local_addr);

                              RESTORE_TOP(local_addr);
                              break;
                           }

    /* I32 | I32 */
    case WASM_OP_I32_ATOMIC_RMW_ADD:
    case WASM_OP_I32_ATOMIC_RMW8_ADD_U:
    case WASM_OP_I32_ATOMIC_RMW16_ADD_U:
    case WASM_OP_I32_ATOMIC_RMW_SUB:
    case WASM_OP_I32_ATOMIC_RMW8_SUB_U:
    case WASM_OP_I32_ATOMIC_RMW16_SUB_U:
    case WASM_OP_I32_ATOMIC_RMW_AND:
    case WASM_OP_I32_ATOMIC_RMW8_AND_U:
    case WASM_OP_I32_ATOMIC_RMW16_AND_U:
    case WASM_OP_I32_ATOMIC_RMW_OR:
    case WASM_OP_I32_ATOMIC_RMW8_OR_U:
    case WASM_OP_I32_ATOMIC_RMW16_OR_U:
    case WASM_OP_I32_ATOMIC_RMW_XOR:
    case WASM_OP_I32_ATOMIC_RMW8_XOR_U:
    case WASM_OP_I32_ATOMIC_RMW16_XOR_U:
    case WASM_OP_I32_ATOMIC_RMW_XCHG:
    case WASM_OP_I32_ATOMIC_RMW8_XCHG_U:
    case WASM_OP_I32_ATOMIC_RMW16_XCHG_U: {
                                            SET_RET (RET_I32);
                                          }
    case WASM_OP_I32_STORE:
    case WASM_OP_I32_STORE8:
    case WASM_OP_I32_STORE16:
    case WASM_OP_I32_ATOMIC_STORE:
    case WASM_OP_I32_ATOMIC_STORE8:
    case WASM_OP_I32_ATOMIC_STORE16: {
                                       SET_RET (RET_VOID);
                                     }
                                     {
                                        BLOCK_INST(sig_i32_2);
                                        SAVE_TOP (local_i32_1);
                                        TEE_TOP (local_addr);
                                        RESTORE_TOP (local_i32_1);

                                        RESTORE_TOP (local_addr);
                                        RESTORE_TOP (local_i32_1);
                                        break;
                                     }

    /* I32 | I64 */
    case WASM_OP_I64_ATOMIC_RMW_ADD:
    case WASM_OP_I64_ATOMIC_RMW8_ADD_U:
    case WASM_OP_I64_ATOMIC_RMW16_ADD_U:
    case WASM_OP_I64_ATOMIC_RMW32_ADD_U:
    case WASM_OP_I64_ATOMIC_RMW_SUB:
    case WASM_OP_I64_ATOMIC_RMW8_SUB_U:
    case WASM_OP_I64_ATOMIC_RMW16_SUB_U:
    case WASM_OP_I64_ATOMIC_RMW32_SUB_U:
    case WASM_OP_I64_ATOMIC_RMW_AND:
    case WASM_OP_I64_ATOMIC_RMW8_AND_U:
    case WASM_OP_I64_ATOMIC_RMW16_AND_U:
    case WASM_OP_I64_ATOMIC_RMW32_AND_U:
    case WASM_OP_I64_ATOMIC_RMW_OR:
    case WASM_OP_I64_ATOMIC_RMW8_OR_U:
    case WASM_OP_I64_ATOMIC_RMW16_OR_U:
    case WASM_OP_I64_ATOMIC_RMW32_OR_U:
    case WASM_OP_I64_ATOMIC_RMW_XOR:
    case WASM_OP_I64_ATOMIC_RMW8_XOR_U:
    case WASM_OP_I64_ATOMIC_RMW16_XOR_U:
    case WASM_OP_I64_ATOMIC_RMW32_XOR_U:
    case WASM_OP_I64_ATOMIC_RMW_XCHG:
    case WASM_OP_I64_ATOMIC_RMW8_XCHG_U:
    case WASM_OP_I64_ATOMIC_RMW16_XCHG_U:
    case WASM_OP_I64_ATOMIC_RMW32_XCHG_U: {
                                            SET_RET (RET_I64);
                                          }
    case WASM_OP_I64_ATOMIC_STORE:
    case WASM_OP_I64_ATOMIC_STORE8:
    case WASM_OP_I64_ATOMIC_STORE16:
    case WASM_OP_I64_ATOMIC_STORE32:
    case WASM_OP_I64_STORE:
    case WASM_OP_I64_STORE8:
    case WASM_OP_I64_STORE16:
    case WASM_OP_I64_STORE32: {
                                SET_RET (RET_VOID);
                              }
                              {
                                BLOCK_INST(sig_i32_i64);
                                SAVE_TOP (local_i64_1);
                                TEE_TOP (local_addr);
                                RESTORE_TOP (local_i64_1);

                                RESTORE_TOP (local_addr);
                                RESTORE_TOP (local_i64_1);
                                break;
                              }

    /* I32 | F32 */
    case WASM_OP_F32_STORE: {
                              BLOCK_INST(sig_i32_f32);
                              SAVE_TOP (local_f32);
                              TEE_TOP (local_addr);
                              RESTORE_TOP (local_f32);

                              RESTORE_TOP (local_addr);
                              RESTORE_TOP (local_f32);
                              break;
                            }

    /* I32 | F64 */
    case WASM_OP_F64_STORE: {
                              BLOCK_INST(sig_i32_f64);
                              SAVE_TOP (local_f64);
                              TEE_TOP (local_addr);
                              RESTORE_TOP (local_f64);

                              RESTORE_TOP (local_addr);
                              RESTORE_TOP (local_f64);
                              break;
                            }

    /* I32 | I32 | I32 */
    case WASM_OP_I32_ATOMIC_RMW_CMPXCHG:
    case WASM_OP_I32_ATOMIC_RMW8_CMPXCHG_U:
    case WASM_OP_I32_ATOMIC_RMW16_CMPXCHG_U: {
                                               SET_RET (RET_I32);
                                             }
                                             {
                                               BLOCK_INST(sig_i32_3);
                                               SAVE_TOP (local_i32_1);
                                               SAVE_TOP (local_i32_2);
                                               TEE_TOP (local_addr);
                                               RESTORE_TOP (local_i32_2);
                                               RESTORE_TOP (local_i32_1);

                                               RESTORE_TOP (local_addr);
                                               RESTORE_TOP (local_i32_2);
                                               RESTORE_TOP (local_i32_1);
                                               break;
                                             }

    /* I32 | I64 | I64 */
    case WASM_OP_I64_ATOMIC_RMW_CMPXCHG:
    case WASM_OP_I64_ATOMIC_RMW8_CMPXCHG_U:
    case WASM_OP_I64_ATOMIC_RMW16_CMPXCHG_U:
    case WASM_OP_I64_ATOMIC_RMW32_CMPXCHG_U: {
                                               SET_RET (RET_I64);
                                             }
                                             {
                                               BLOCK_INST(sig_i32_i64_2);
                                               SAVE_TOP (local_i64_1);
                                               SAVE_TOP (local_i64_2);
                                               TEE_TOP (local_addr);
                                               RESTORE_TOP (local_i64_2);
                                               RESTORE_TOP (local_i64_1);

                                               RESTORE_TOP (local_addr);
                                               RESTORE_TOP (local_i64_2);
                                               RESTORE_TOP (local_i64_1);
                                               break;
                                             }

    /* Synchronization Ops: Currently do not duplicate
     * However, we need to record it in the trace.. */
    case WASM_OP_MEMORY_ATOMIC_NOTIFY:
    case WASM_OP_MEMORY_ATOMIC_WAIT32:
    case WASM_OP_MEMORY_ATOMIC_WAIT64: {
                                         break;
                                       }
    default: DEFAULT_ERR_CASE()
  }

  mem_offset = mem_inst->getOffset();

  MEMOP_SETUP_END(ImmMemargInst,
    new_meminst->setMemory(record_mem)
  );

  return addinst;

}


bool import_exclude_list(FuncDecl *target, WasmModule &module) {
  if (
    //(target == module.find_import_func("wali", "__wasm_thread_spawn")) || 
    (target == module.find_import_func("wali", "SYS_futex"))
  ) {
    return true;
  } else {
    return false;
  }
}

InstList setup_call_record_instrument (std::list<InstBasePtr>::iterator &itr, 
    uint32_t local_idxs[11], uint32_t sig_idxs[7], 
    std::map<FuncDecl*, std::string> &specialized_calls, 
    std::map<FuncDecl*, std::string> &lockless_calls,
    CallInstInfo &recinfo, MemoryDecl *def_mem, MemoryDecl *record_mem, 
    WasmModule &module) {

  CALL_SETUP_INIT(ImmFuncInst);

  FuncDecl *target = call_inst->getFunc();
  CallID call_id = SC_UNKNOWN;
  std::string mod_name;
  std::string member_name;
  std::vector<int64_t> args;

  switch (opcode) {
    /*** ImmFunc type ***/
    case WASM_OP_CALL:  {
      auto spec_it = specialized_calls.find(target);
      auto ll_it = lockless_calls.find(target);
      if (spec_it != specialized_calls.end()) {
        /* Handle return local + instructions for specialized calls */
        std::string call_name = spec_it->second;
        SET_RET (RET_I64);
        if (call_name == "SYS_mmap") {
          call_id = SC_MMAP;
          // This is just to force us to trace the call
          PUSH_INST (NopInst());
        }
        else if (call_name == "SYS_writev") {
          call_id = SC_WRITEV;
          PUSH_INST (LocalSetInst(local_i32_3));
          PUSH_INST (LocalSetInst(local_i32_2));
          PUSH_INST (LocalTeeInst(local_i32_1));
          PUSH_INST (LocalGetInst(local_i32_2));
          PUSH_INST (LocalGetInst(local_i32_3));
        }
      }
      //else if (ll_it != lockless_calls.end()) {
      //  /* Handle return local for lockless calls */
      //  std::string call_name = ll_it->second;
      //  if (call_name == "__wasm_thread_spawn") {
      //    call_id = SC_THREAD_SPAWN;
      //  }
      //  else if (call_name == "SYS_futex") {
      //    call_id = SC_FUTEX;
      //  }
      //  else if (call_name == "SYS_exit") {
      //    call_id = SC_EXIT;
      //  }
      //}
      else if (module.isImport(target) && (ll_it == lockless_calls.end())) {
        /* Handle return local for generic import calls */
        call_id = SC_GENERIC;
        typelist result_types = target->sig->results;
        if (result_types.size() > 1) {
          ERR("R3-Record-Error: Unsupported return types larger than 1 "
            "Got %lu for Func[%d]\n", result_types.size(), module.getFuncIdx(target));
        }
        else if (result_types.size() == 0) {
          SET_RET (RET_VOID);
        }
        else {
          switch (result_types.front()) {
            case WASM_TYPE_I32: SET_RET (RET_I32); break;
            case WASM_TYPE_I64: SET_RET (RET_I64); break;
            case WASM_TYPE_F32: SET_RET (RET_F32); break;
            case WASM_TYPE_F64: SET_RET (RET_F64); break;
            default: { ERR("R3-Record-Error: Unsupported return type %d (opc: %04X)\n", 
                        result_types.front(), opcode); }
          }
        }
        // This is just to force us to trace the call
        PUSH_INST (NopInst());
      }
      else {
        /* Handle return for internal calls */

      }
      break;
    }

    default: DEFAULT_ERR_CASE();
  }

  CALL_SETUP_END(ImmFuncInst, {});
  return addinst;
}


/* Typecast any value to I64 */
#define PUSH_I64_EXTEND(type) {                                      \
  switch (type) {                                                    \
    case RET_I32: PUSH_INST (I64ExtendI32UInst()); break;            \
    case RET_I64: break;                                             \
    case RET_F32: PUSH_INST (I32ReinterpretF32Inst());               \
                  PUSH_INST (I64ExtendI32UInst()); break;            \
    case RET_F64: PUSH_INST (I64ReinterpretF64Inst()); break;        \
    default: { ERR("R3-Record-Error: Unsupported type %d for I64 Extend (opc: %04X)\n", \
      type, opcode); } \
  }                                                                  \
}

#define LOCAL_I64_EXTEND(local, type) { \
  PUSH_INST (LocalGetInst(local)); \
  PUSH_I64_EXTEND(type); \
}

#define I64_SUCCESS_CHECK(code) { \
  PUSH_INST (BlockInst(sig_indices[1])); \
  PUSH_INST (LocalTeeInst(i64_tmp_local_1)); \
  PUSH_INST (LocalGetInst(i64_tmp_local_1)); \
  PUSH_INST (I64ConstInst(0)); \
  PUSH_INST (I64LtSInst()); \
  PUSH_INST (BrIfInst(0)); \
  { code } \
  PUSH_INST (EndInst()); \
}

InstList gen_call_trace_instructions(InstBasePtr &instruction, uint32_t access_idx, CallInstInfo &recinfo,
    FuncDecl *tracedump_fn, uint32_t sig_indices[4], uint32_t local_idxs[19],
    MemoryDecl *def_mem, MemoryDecl *record_mem, WasmModule &module)  {

  InstList addinst;
  uint16_t opcode = instruction->getOpcode();
  CallID call_id = recinfo.call_id;

  uint32_t local_f64 = local_idxs[0];
  uint32_t local_f32 = local_idxs[1];
  uint32_t local_i64_1 = local_idxs[2];
  uint32_t local_i64_2 = local_idxs[3];
  uint32_t local_i32_1 = local_idxs[4];
  uint32_t local_i32_2 = local_idxs[5];
  uint32_t local_i32_3 = local_idxs[6];
  uint32_t local_i32_4 = local_idxs[11];
  uint32_t local_i64_3 = local_idxs[12];
  uint32_t local_i32_5 = local_idxs[15];
  uint32_t i64_tmp_local_1 = local_idxs[17];
  uint32_t i64_tmp_local_2 = local_idxs[18];

  RetVal ret = recinfo.ret;
  bool has_ret = (ret.type != RET_VOID);

  // Instrumentation for after the call (but before trace data)
  // Default (common) case involves getting the return value
  switch (call_id) {
    case SC_UNKNOWN: {
      ERR("R3-Record-Error: Call ID %d not expected\n", call_id);
      break;
    }
    // mmap requires extra operation in addition to default for generic calls
    // local_i32_1 records memory grow length
    case SC_MMAP: {
      I64_SUCCESS_CHECK(
        PUSH_INST (MemorySizeInst(def_mem));
        PUSH_INST (MemorySizeInst(record_mem));
        PUSH_INST (I32SubInst());
        PUSH_INST (LocalTeeInst(local_i32_1));
        PUSH_INST (MemoryGrowInst(record_mem));
        PUSH_INST (DropInst());
      );
    }
    default: {
      // Default case: Save return value if it has one
      if (has_ret) {
        PUSH_INST (LocalTeeInst(ret.local));
      }
      break; 
    }
  }

  /* Acc Idx */
  PUSH_INST (I32ConstInst(access_idx));
  /* Opcode */
  PUSH_INST (I32ConstInst(opcode));
  /* Function Index */
  uint32_t func_idx = module.getFuncIdx(recinfo.target);
  PUSH_INST (I32ConstInst(func_idx));
  /* Call ID */
  PUSH_INST (I32ConstInst(call_id));
  /* Return Value: For values without ret, it can be resolved at replay gen */
  if (has_ret) {
    PUSH_INST (LocalGetInst(ret.local));
    PUSH_I64_EXTEND(ret.type);
  } else {
    PUSH_INST (I64ConstInst(0));
  }

  /* Args for specific call */
  int num_args = 0;
  switch (call_id) {
    case SC_MMAP: {
      num_args = 1;
      LOCAL_I64_EXTEND(local_i32_1, RET_I32);
      break;
    }
    case SC_WRITEV: {
      num_args = 3;
      LOCAL_I64_EXTEND(local_i32_1, RET_I32);
      LOCAL_I64_EXTEND(local_i32_2, RET_I32);
      LOCAL_I64_EXTEND(local_i32_3, RET_I32);
      break;
    }
    case SC_GENERIC: { break; }
    default: { ERR("R3-Record-Error: Unsupported call ID %d\n", call_id); }
  }
  // Push remaining placeholder args
  for (int i = num_args; i < 3; i++) {
    PUSH_INST (I64ConstInst(0));
  }

  /* Tracedump call */
  PUSH_INST (CallInst(tracedump_fn));
  
  return addinst;
}

InstList gen_memop_trace_instructions(InstBasePtr &instruction, uint32_t access_idx, MemopInstInfo &recinfo,
    FuncDecl *tracedump_fn, uint32_t local_ret_idxs[8], MemoryDecl *record_mem) {
  InstList addinst;
  RetVal ret = recinfo.ret;
  /* Perform comparison operator */
  InstBasePtr neq, load_inst;
  uint32_t main_value_local = local_ret_idxs[ret.type - RET_I32];
  uint32_t i64_local = local_ret_idxs[RET_I64 - RET_I32];
  uint32_t full_addr_local = local_ret_idxs[4];
  uint32_t differ_local = local_ret_idxs[5];
  uint32_t i64_tmp_local_1 = local_ret_idxs[6];
  uint32_t i64_tmp_local_2 = local_ret_idxs[7];
  uint16_t opcode = instruction->getOpcode();
  bool no_ret = false;

  switch (ret.type) {
    case RET_I32: { neq.reset(new I32NeInst()); break; }
    case RET_I64: { neq.reset(new I64NeInst()); break; }
    case RET_F32: { neq.reset(new F32NeInst()); break; }
    case RET_F64: { neq.reset(new F64NeInst()); break; }
    default: { no_ret = true; }
  }

  /* Differ */
  // Only values that return trigger a difference writeback
  if (no_ret) {
    PUSH_INST (I32ConstInst(0));
  } else {
    PUSH_INST (LocalTeeInst(main_value_local));
    PUSH_INST (LocalGetInst(main_value_local));
    PUSH_INST (LocalGetInst(ret.local));
    PUSH_INST_PTR (neq);
  }
  PUSH_INST (LocalTeeInst(differ_local));

  /* Acc Idx */
  PUSH_INST (I32ConstInst(access_idx));
  /* Opcode */
  PUSH_INST (I32ConstInst(opcode));

  /* Addr for memops: Save in local too */
  PUSH_INST (I32ConstInst(recinfo.offset));
  PUSH_INST (LocalGetInst(recinfo.base_addr_local));
  PUSH_INST (I32AddInst());
  PUSH_INST (LocalTeeInst(full_addr_local));

  /* Size */
  if (recinfo.prop.size != -1) {
    PUSH_INST (I32ConstInst(recinfo.prop.size));
  } else {
    PUSH_INST (LocalGetInst(recinfo.accwidth_local));
  }

  if (!no_ret) {
    /* Main-Memory Load Value with storeback to shadow */
    PUSH_INST (BlockInst(-64));
    PUSH_INST (LocalGetInst(differ_local));
    PUSH_INST (I32EqzInst());
    PUSH_INST (BrIfInst(0));

    PUSH_INST (LocalGetInst(full_addr_local));
    PUSH_INST (LocalGetInst(main_value_local));
    // Typecast any value to I64
    PUSH_I64_EXTEND(ret.type);
    // Store back main value to shadow memory
    PUSH_INST (LocalTeeInst(i64_local));
    switch (memop_inst_table[opcode].size) {
      case 1: PUSH_INST (I64Store8Inst(0, 0, record_mem)); break;
      case 2: PUSH_INST (I64Store16Inst(0, 0, record_mem)); break;
      case 4: PUSH_INST (I64Store32Inst(0, 0, record_mem)); break;
      case 8: PUSH_INST (I64StoreInst(0, 0, record_mem)); break;
      default: { ERR("R3-Record-Error: Unsupported size %ld for %04X\n", 
        memop_inst_table[opcode].size, opcode); }
    }
    PUSH_INST (LocalGetInst(i64_local));

    /* Expected Main-Memory Load Value */
    PUSH_INST (LocalGetInst(ret.local));
    PUSH_I64_EXTEND(ret.type);

    PUSH_INST (LocalSetInst(i64_tmp_local_1));
    PUSH_INST (LocalSetInst(i64_tmp_local_2));
    PUSH_INST (EndInst());
    PUSH_INST (LocalGetInst(i64_tmp_local_2));
    PUSH_INST (LocalGetInst(i64_tmp_local_1));
  } else {
    PUSH_INST (I64ConstInst(0));
    PUSH_INST (I64ConstInst(0));
  }

  /* Tracedump call */
  PUSH_INST (CallInst(tracedump_fn));

  return addinst;
}

/* Main R3 Record function */
void r3_record_instrument (WasmModule &module) {
  /* Engine callback for tracing */
  /* Tracing Parameters:
   *    Differ? (I32)
   *    Access Index (I32)
   *    Opcode (I32)
   *    Base Address (I32)
   *    Size of access (I32)
   *    Main-Memory Load Value (I32/optional)
   * Return:
   *    None
  */
  ImportInfo iminfo;
  SigDecl tracedump_sig;

  iminfo = {
    .mod_name = "instrument",
    .member_name = "memop_tracedump"
  };
  tracedump_sig = {
    .params = {
      // Differ?
      WASM_TYPE_I32, 
      // Access Index
      WASM_TYPE_I32, 
      // Opcode
      WASM_TYPE_I32, 
      // Address
      WASM_TYPE_I32, 
      // Size/Width of Access
      WASM_TYPE_I32,
      // Load value from Main Memory (only used if Differ == 1) 
      WASM_TYPE_I64, 
      // Expected value from Main Memory (only used if Differ == 1) 
      WASM_TYPE_I64
    },
    .results = {} 
  };
  ImportDecl* memop_tracedump_import_decl = module.add_import(iminfo, tracedump_sig);
  FuncDecl* memop_tracedump_fn = memop_tracedump_import_decl->desc.func;

  iminfo = {
    .mod_name = "instrument",
    .member_name = "call_tracedump"
  };
  tracedump_sig = {
    .params = {
      // Access Index
      WASM_TYPE_I32, 
      // Opcode
      WASM_TYPE_I32, 
      // Function Index
      WASM_TYPE_I32, 
      // Call ID
      WASM_TYPE_I32,
      // Return value (up to 1)
      WASM_TYPE_I64,
      // Arguments (up to 3)
      WASM_TYPE_I64, WASM_TYPE_I64, WASM_TYPE_I64,
    },
    .results = {} 
  };
  ImportDecl* call_tracedump_import_decl = module.add_import(iminfo, tracedump_sig);
  FuncDecl* call_tracedump_fn = call_tracedump_import_decl->desc.func;

  /* Create custom mutex lock/unlock functions */
  FuncDecl *mutex_funcs[2];
  uint32_t mutex_addr = (add_pages(module, 1) * WASM_PAGE_SIZE);
  create_helper_funcs(module, mutex_addr, mutex_funcs);

  FuncDecl *lock_fn = mutex_funcs[0];
  FuncDecl *unlock_fn = mutex_funcs[1];

  /* Create new memory: Must happen after adding all instrumentation pages */
  MemoryDecl *def_mem = module.getMemory(0);
  MemoryDecl new_mem = *def_mem;

  MemoryDecl *record_mem = module.add_memory(new_mem);

  /* Generate quick lookup of ignored exported function idxs */
  std::map<FuncDecl*, std::string> func_export_map;
  set_func_export_map(module, "wasm_memory_grow", func_export_map);
  set_func_export_map(module, "wasm_memory_size", func_export_map);

  /* Pre-compute sig indices since list indexing is expensive */
  uint32_t sig_indices[7];
  typelist blocktypes[7] = {
    {WASM_TYPE_I32}, // Addr
    {WASM_TYPE_I32, WASM_TYPE_I32}, // Addr | I32
    {WASM_TYPE_I32, WASM_TYPE_I64}, // Addr | I64
    {WASM_TYPE_I32, WASM_TYPE_F32}, // Addr | F32
    {WASM_TYPE_I32, WASM_TYPE_F64}, // Addr | F64
    {WASM_TYPE_I32, WASM_TYPE_I32, WASM_TYPE_I32}, // Addr | I32 | I32
    {WASM_TYPE_I32, WASM_TYPE_I64, WASM_TYPE_I64}, // Addr | I64 | I64
  };
  for (int i = 0; i < 7; i++) {
    SigDecl s = { .params = blocktypes[i], .results = blocktypes[i] };
    sig_indices[i] = module.getSigIdx(module.add_sig(s, false));
  }

  SigDecl s = {.params = {WASM_TYPE_I64}, .results = {}};
  uint32_t call_trace_sig_indices[4];
  typelist call_trace_blocktypes[4] = {
    {WASM_TYPE_I32},
    {WASM_TYPE_I64},
    {WASM_TYPE_F32},
    {WASM_TYPE_F64}
  };
  for (int i = 0; i < 4; i++) {
    SigDecl s = { .params = call_trace_blocktypes[i], .results = call_trace_blocktypes[i] };
    call_trace_sig_indices[i] = module.getSigIdx(module.add_sig(s, false));
  }

  /* Import calls that require extra non-generic instrumentation
    This still use lock-based instrumentation */
  std::map<FuncDecl*, std::string> specialized_calls;
  {
    const char *calls[2] = {"SYS_mmap", "SYS_writev"};
    for (int i = 0; i < 2; i++) {
      if (FuncDecl *f = module.find_import_func("wali", calls[i]))
        specialized_calls[f] = calls[i];
    }
  }
  /* Import calls that are excluded from lock-based instrumentation 
     Each are handled in a specialized manner */
  std::map<FuncDecl*, std::string> lockless_calls;
  {
    const char *calls[3] = {"__wasm_thread_spawn", "SYS_futex", "SYS_exit"};
    for (int i = 0; i < 3; i++) {
      if (FuncDecl *f = module.find_import_func("wali", calls[i]))
        lockless_calls[f] = calls[i];
    }
  }


  uint32_t access_tracker = 1;
  /* Instrument all functions */
  for (auto &func : module.Funcs()) {
    /* Do not instrument the instrument-hook functions */
    if ((&func == lock_fn) || (&func == unlock_fn) || (func_export_map.count(&func))) {
      continue;
    }
    uint32_t local_indices[19] = {
      /* For Call, all these locals are for call arg shepherding
         For Memop, they are defined below */
      func.add_local(WASM_TYPE_F64),
      func.add_local(WASM_TYPE_F32),
      func.add_local(WASM_TYPE_I64),
      func.add_local(WASM_TYPE_I64),
      func.add_local(WASM_TYPE_I32),
      func.add_local(WASM_TYPE_I32),
      func.add_local(WASM_TYPE_I32),
      /* Save values for shadow loads */
      func.add_local(WASM_TYPE_I32),
      func.add_local(WASM_TYPE_I64),
      func.add_local(WASM_TYPE_F32),
      func.add_local(WASM_TYPE_F64),
      /* Save values for original loads */
      func.add_local(WASM_TYPE_I32),
      func.add_local(WASM_TYPE_I64),
      func.add_local(WASM_TYPE_F32),
      func.add_local(WASM_TYPE_F64),
      /* Trace Temp value for address */
      func.add_local(WASM_TYPE_I32),
      /* Trace Temp value for differ */
      func.add_local(WASM_TYPE_I32),
      /* Trace Temp for load values / call args */
      func.add_local(WASM_TYPE_I64),
      func.add_local(WASM_TYPE_I64),
    };
    InstList &insts = func.instructions;

    /** Instrumenting memory-related instructions **/
    for (auto institr = insts.begin(); institr != insts.end(); ++institr) {
      InstBasePtr &instruction = *institr;
      bool inc_access_tracker = true;
      RecordInstInfo record {};
      memset(&record, 0, sizeof(RecordInstInfo));
      InstList addinst;
#define SETUP_INVOKE(ty) setup_##ty##_record_instrument(institr, local_indices, sig_indices, record_mem, \
    access_tracker, record.Memop)
      switch(instruction->getImmType()) {
        case IMM_MEMARG: 
          addinst = SETUP_INVOKE(memarg);
          break;
        case IMM_MEMARG_LANEIDX: 
          addinst = SETUP_INVOKE(memarg_laneidx);
          break;
        case IMM_MEMORY:
          addinst = SETUP_INVOKE(memory);
          break;
        case IMM_DATA_MEMORY:
          addinst = SETUP_INVOKE(data_memory);
          break;
        case IMM_MEMORYCP:
          addinst = SETUP_INVOKE(memorycp);
          break;
        /* Calls: Lock those to select import functions */
        case IMM_FUNC: 
          addinst = setup_call_record_instrument(institr, local_indices, sig_indices, 
              specialized_calls, lockless_calls, record.Call, 
              def_mem, record_mem, module);
          break;
        ///* Indirect Calls: Lock none */
        //case IMM_SIG_TABLE: {
        //                      PUSH_INST(NopInst()); break;
        //                    }
        default: { inc_access_tracker = false; }; 
      }
#undef SETUP_INVOKE
      if (!addinst.empty()) {
        auto next_institr = std::next(institr);
        /* Insert instructions guarded by mutex (pre/post) */
        InstList preinst, postinst;

        InstList traceinst;
        switch (instruction->getImmType()) {
          case IMM_MEMARG_LANEIDX:
          case IMM_MEMORY:
          case IMM_MEMORYCP:
          case IMM_DATA_MEMORY:
          case IMM_MEMARG: {
            traceinst = gen_memop_trace_instructions(instruction, 
              access_tracker, record.Memop, memop_tracedump_fn, &local_indices[11], 
              record_mem);
            break;
          }

          case IMM_FUNC: {
            traceinst = gen_call_trace_instructions(instruction,
              access_tracker, record.Call, call_tracedump_fn, 
              call_trace_sig_indices, local_indices, 
              def_mem, record_mem, module);
            break;
          }
          default: {
                    ERR("R3-Record-Error: Missupported Imm Type %d\n", instruction->getImmType()); \
                  }; 
        }
        InstBasePtr lock_inst = InstBasePtr(new CallInst(lock_fn));
        InstBasePtr unlock_inst = InstBasePtr(new CallInst(unlock_fn));

        preinst.push_back(lock_inst);
        postinst.insert(postinst.begin(), traceinst.begin(), traceinst.end());
        postinst.push_back(unlock_inst);
        //if (post_insert) {
        //  postinst.insert(postinst.begin(), addinst.begin(), addinst.end());
        //} 
        //else {
          preinst.insert(preinst.end(), addinst.begin(), addinst.end());
        //}
        insts.insert(institr, preinst.begin(), preinst.end());
        insts.insert(next_institr, postinst.begin(), postinst.end());
        /* Don't iterate over our inserted instructions */
        institr = std::prev(next_institr);
      }
      if (inc_access_tracker) {
        access_tracker++;
      }
    }
  }

}
