#include "routine_common.h"

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


#define SETUP_INIT(_insttype)                                                        \
  uint32_t local_f64 = local_idxs[0];                                                \
  uint32_t local_f32 = local_idxs[1];                                                \
  uint32_t local_i64_1 = local_idxs[2];                                              \
  uint32_t local_i64_2 = local_idxs[3];                                              \
  uint32_t local_i32_1 = local_idxs[4];                                              \
  uint32_t local_i32_2 = local_idxs[5];                                              \
  uint32_t local_addr = local_idxs[6];                                               \
                                                                                     \
  uint32_t sig_i32 = sig_idxs[0];                                                    \
  uint32_t sig_i32_2 = sig_idxs[1];                                                  \
  uint32_t sig_i32_i64 = sig_idxs[2];                                                \
  uint32_t sig_i32_f32 = sig_idxs[3];                                                \
  uint32_t sig_i32_f64 = sig_idxs[4];                                                \
  uint32_t sig_i32_3 = sig_idxs[5];                                                  \
  uint32_t sig_i32_i64_2 = sig_idxs[6];                                              \
                                                                                     \
  InstBasePtr instruction = (*itr);                                                  \
  std::shared_ptr<_insttype> mem_inst = static_pointer_cast<_insttype>(instruction); \
                                                                                     \
  InstList addinst;                                                                  \
  bool insblock = false;                                                             \
  int stackrets = 0;                                                                 \
  uint16_t opcode = instruction->getOpcode();


#define SETUP_END(_insttype, updater)                  \
  if (insblock) {                                      \
    _insttype *new_meminst = new _insttype(*mem_inst); \
    { updater; }                                       \
    PUSH_INST_PTR (new_meminst);                       \
    for (int i = 0; i < stackrets; i++) {              \
      PUSH_INST (DropInst());                          \
    }                                                  \
    PUSH_INST (EndInst());                             \
  }


#define DEFAULT_ERR_CASE() {  \
  ERR("R3-Record-Error: Cannot support opcode %04X (%s)\n", opcode, opcode_table[opcode].mnemonic); \
}

InstList setup_memarg_laneidx_record_instrument (std::list<InstBasePtr>::iterator &itr, 
    uint32_t local_idxs[7], uint32_t sig_idxs[7], MemoryDecl *record_mem) {

  SETUP_INIT(ImmMemargLaneidxInst);

  switch (opcode) {
    /*** ImmMemargLaneidx type ***/
    default: DEFAULT_ERR_CASE()
  }

  SETUP_END(ImmMemargLaneidxInst,
    new_meminst->setMemory(record_mem)
  );

  return addinst;
}


InstList setup_memory_record_instrument (std::list<InstBasePtr>::iterator &itr, 
    uint32_t local_idxs[7], uint32_t sig_idxs[7], MemoryDecl *record_mem) {

  SETUP_INIT(ImmMemoryInst);

  switch (opcode) {
    /*** ImmMemory type ***/
    /* I32 | I32 | I32 */
    case WASM_OP_MEMORY_FILL: {
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
                                 stackrets = 1;
                                 break;
                              }
    /* I32 */
    case WASM_OP_MEMORY_GROW: {
                                BLOCK_INST(sig_i32);
                                TEE_TOP(local_addr);

                                RESTORE_TOP(local_addr);
                                stackrets = 1;
                                break;
                              }
    default: DEFAULT_ERR_CASE()
  }

  SETUP_END(ImmMemoryInst,
    new_meminst->setMemory(record_mem)
  );

  return addinst;
}


InstList setup_data_memory_record_instrument (std::list<InstBasePtr>::iterator &itr, 
    uint32_t local_idxs[7], uint32_t sig_idxs[7], MemoryDecl *record_mem) {

  SETUP_INIT(ImmDataMemoryInst);

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

  SETUP_END(ImmDataMemoryInst,
    new_meminst->setMemory(record_mem)
  );

  return addinst;
}


InstList setup_memorycp_record_instrument (std::list<InstBasePtr>::iterator &itr, 
    uint32_t local_idxs[7], uint32_t sig_idxs[7], MemoryDecl *record_mem) {

  SETUP_INIT(ImmMemorycpInst);

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

  SETUP_END(ImmMemorycpInst,
    new_meminst->setDstMemory(record_mem)
  );

  return addinst;
}


InstList setup_memarg_record_instrument (std::list<InstBasePtr>::iterator &itr, 
    uint32_t local_idxs[7], uint32_t sig_idxs[7], MemoryDecl *record_mem) {

  SETUP_INIT(ImmMemargInst);

  switch (opcode) {
    /*** ImmMemarg type ***/
    /* I32 */
    case WASM_OP_I32_LOAD:
    case WASM_OP_I64_LOAD:
    case WASM_OP_F32_LOAD:
    case WASM_OP_F64_LOAD:
    case WASM_OP_I32_LOAD8_S:
    case WASM_OP_I32_LOAD8_U:
    case WASM_OP_I32_LOAD16_S:
    case WASM_OP_I32_LOAD16_U:
    case WASM_OP_I64_LOAD8_S:
    case WASM_OP_I64_LOAD8_U:
    case WASM_OP_I64_LOAD16_S:
    case WASM_OP_I64_LOAD16_U:
    case WASM_OP_I64_LOAD32_S:
    case WASM_OP_I64_LOAD32_U:
    case WASM_OP_I32_ATOMIC_LOAD:
    case WASM_OP_I64_ATOMIC_LOAD:
    case WASM_OP_I32_ATOMIC_LOAD8_U:
    case WASM_OP_I32_ATOMIC_LOAD16_U:
    case WASM_OP_I64_ATOMIC_LOAD8_U:
    case WASM_OP_I64_ATOMIC_LOAD16_U:
    case WASM_OP_I64_ATOMIC_LOAD32_U: {
                                        BLOCK_INST(sig_i32);
                                        TEE_TOP(local_addr);

                                        RESTORE_TOP(local_addr);
                                        stackrets = 1;
                                        break;
                                      }

    /* I32 | I32 */
    case WASM_OP_I32_STORE:
    case WASM_OP_I32_STORE8:
    case WASM_OP_I32_STORE16:
    case WASM_OP_I32_ATOMIC_STORE:
    case WASM_OP_I32_ATOMIC_STORE8:
    case WASM_OP_I32_ATOMIC_STORE16: {
                                        BLOCK_INST(sig_i32_2);
                                        SAVE_TOP (local_i32_1);
                                        TEE_TOP (local_addr);
                                        RESTORE_TOP (local_i32_1);

                                        RESTORE_TOP (local_addr);
                                        RESTORE_TOP (local_i32_1);
                                        break;
                                     }
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
                                            BLOCK_INST(sig_i32_2);
                                            SAVE_TOP (local_i32_1);
                                            TEE_TOP (local_addr);
                                            RESTORE_TOP (local_i32_1);

                                            RESTORE_TOP (local_addr);
                                            RESTORE_TOP (local_i32_1);
                                            stackrets = 1;
                                            break;
                                          }

    /* I32 | I64 */
    case WASM_OP_I64_STORE:
    case WASM_OP_I64_STORE8:
    case WASM_OP_I64_STORE16:
    case WASM_OP_I64_STORE32: {
                                BLOCK_INST(sig_i32_i64);
                                SAVE_TOP (local_i64_1);
                                TEE_TOP (local_addr);
                                RESTORE_TOP (local_i64_1);

                                RESTORE_TOP (local_addr);
                                RESTORE_TOP (local_i64_1);
                                break;
                              }
    case WASM_OP_I64_ATOMIC_STORE:
    case WASM_OP_I64_ATOMIC_STORE8:
    case WASM_OP_I64_ATOMIC_STORE16:
    case WASM_OP_I64_ATOMIC_STORE32:
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
                                            BLOCK_INST(sig_i32_i64);
                                            SAVE_TOP (local_i64_1);
                                            TEE_TOP (local_addr);
                                            RESTORE_TOP (local_i64_1);

                                            RESTORE_TOP (local_addr);
                                            RESTORE_TOP (local_i64_1);
                                            stackrets = 1;
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
                                               BLOCK_INST(sig_i32_3);
                                               SAVE_TOP (local_i32_1);
                                               SAVE_TOP (local_i32_2);
                                               TEE_TOP (local_addr);
                                               RESTORE_TOP (local_i32_2);
                                               RESTORE_TOP (local_i32_1);

                                               RESTORE_TOP (local_addr);
                                               RESTORE_TOP (local_i32_2);
                                               RESTORE_TOP (local_i32_1);
                                               stackrets = 1;
                                               break;
                                             }

    /* I32 | I64 | I64 */
    case WASM_OP_I64_ATOMIC_RMW_CMPXCHG:
    case WASM_OP_I64_ATOMIC_RMW8_CMPXCHG_U:
    case WASM_OP_I64_ATOMIC_RMW16_CMPXCHG_U:
    case WASM_OP_I64_ATOMIC_RMW32_CMPXCHG_U: {
                                               BLOCK_INST(sig_i32_i64_2);
                                               SAVE_TOP (local_i64_1);
                                               SAVE_TOP (local_i64_2);
                                               TEE_TOP (local_addr);
                                               RESTORE_TOP (local_i64_2);
                                               RESTORE_TOP (local_i64_1);

                                               RESTORE_TOP (local_addr);
                                               RESTORE_TOP (local_i64_2);
                                               RESTORE_TOP (local_i64_1);
                                               stackrets = 1;
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

  SETUP_END(ImmMemargInst,
    new_meminst->setMemory(record_mem)
  );

  return addinst;

}

/* Add pages of memory statically. Return the start page */
uint32_t add_pages(WasmModule &module, uint32_t num_pages) {
  wasm_limits_t &memlimit = module.getMemory(0)->limits;
  uint32_t memdata_end = memlimit.initial * PAGE_SIZE;

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

  funcs[0] = module.add_func(fn1, NULL, "lock_hostcall");
  funcs[1] = module.add_func(fn2, NULL, "unlock_hostcall");
}


/* Main R3 Record function */
void r3_record_instrument (WasmModule &module) {
  MemoryDecl *def_mem = module.getMemory(0);
  MemoryDecl new_mem = *def_mem;

  MemoryDecl *record_mem = module.add_memory(new_mem);

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

  /* Create custom mutex lock/unlock functions */
  FuncDecl *mutex_funcs[2];
  uint32_t mutex_addr = (add_pages(module, 1) * PAGE_SIZE);
  create_helper_funcs(module, mutex_addr, mutex_funcs);

  FuncDecl *lock_fn = mutex_funcs[0];
  FuncDecl *unlock_fn = mutex_funcs[1];

  /* Instrument all functions */
  for (auto &func : module.Funcs()) {
    /* Do not instrument the instrument-hook functions */
    if ((&func == lock_fn) || (&func == unlock_fn)) {
      continue;
    }
    uint32_t local_indices[7] = {
      func.add_local(WASM_TYPE_F64),
      func.add_local(WASM_TYPE_F32),
      func.add_local(WASM_TYPE_I64),
      func.add_local(WASM_TYPE_I64),
      func.add_local(WASM_TYPE_I32),
      func.add_local(WASM_TYPE_I32),
      func.add_local(WASM_TYPE_I32)
    };
    InstList &insts = func.instructions;
    for (auto institr = insts.begin(); institr != insts.end(); ++institr) {
      InstBasePtr &instruction = *institr;
      InstList addinst;
#define SETUP_CALL(ty) setup_##ty##_record_instrument(institr, local_indices, sig_indices, record_mem)
      switch(instruction->getImmType()) {
        case IMM_MEMARG: 
          addinst = SETUP_CALL(memarg);
          break;
        case IMM_MEMARG_LANEIDX: 
          addinst = SETUP_CALL(memarg_laneidx);
          break;
        case IMM_MEMORY:
          addinst = SETUP_CALL(memory);
          break;
        case IMM_DATA_MEMORY:
          addinst = SETUP_CALL(data_memory);
          break;
        case IMM_MEMORYCP:
          addinst = SETUP_CALL(memorycp);
          break;
        default: {}; 
      }
#undef SETUP_CALL
      if (!addinst.empty()) {
        /* Insert instructions guarded by mutex */
        InstBasePtr lock_inst = InstBasePtr(new CallInst(lock_fn));
        InstBasePtr unlock_inst = InstBasePtr(new CallInst(unlock_fn));
        addinst.push_front(lock_inst);
        insts.insert(institr, addinst.begin(), addinst.end());
        insts.insert(std::next(institr), unlock_inst);
      }
    }
  }

}
