#include "routine_common.h"

/*****************/
void memaccess_instrument (WasmModule &module, const std::string& path);

std::vector<WasmModule> memaccess_stochastic_instrument (WasmModule &module, 
    int percent, int cluster_size, const std::string& path);

std::vector<WasmModule> memaccess_balanced_instrument (WasmModule &module, 
    int cluster_size, const std::string& path);
/****************/


InstList setup_logappend_args (std::list<InstBasePtr>::iterator &itr, 
    uint32_t local_idxs[7], FuncDecl *logaccess_import, uint32_t access_idx) {

  uint32_t local_f64 = local_idxs[0];
  uint32_t local_f32 = local_idxs[1];
  uint32_t local_i64_1 = local_idxs[2];
  uint32_t local_i64_2 = local_idxs[3];
  uint32_t local_i32_1 = local_idxs[4];
  uint32_t local_i32_2 = local_idxs[5];
  uint32_t local_addr = local_idxs[6];

  InstBasePtr instruction = (*itr);
  std::shared_ptr<ImmMemargInst> mem_inst = static_pointer_cast<ImmMemargInst>(instruction);
  
  InstList addinst;
  #define PUSH_INST(inv)   addinst.push_back(InstBasePtr(new inv));
  #define SAVE_TOP(var) { \
    PUSH_INST (LocalSetInst(var)); \
  }
  #define RESTORE_TOP(var) { \
    PUSH_INST (LocalGetInst(var)); \
  }
  #define IC_ROUTINE() {  \
    PUSH_INST (LocalTeeInst(local_addr));  \
    PUSH_INST (LocalGetInst(local_addr));  \
    PUSH_INST (I32ConstInst(mem_inst->getOffset())); \
    PUSH_INST (I32AddInst());  \
    /* Get opcode of mem access */ \
    PUSH_INST (I32ConstInst(instruction->getOpcode()));  \
    /* Assign index to access access */ \
    PUSH_INST (I32ConstInst(access_idx));  \
    PUSH_INST (CallInst(logaccess_import)); \
  }

  switch (instruction->getOpcode()) {
    /* Addr */
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
                                        IC_ROUTINE();
                                        break;
                                      }

    /* Addr | I32 */
    case WASM_OP_I32_STORE:
    case WASM_OP_I32_STORE8:
    case WASM_OP_I32_STORE16:
    case WASM_OP_I32_ATOMIC_STORE:
    case WASM_OP_I32_ATOMIC_STORE8:
    case WASM_OP_I32_ATOMIC_STORE16:
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
                                            SAVE_TOP (local_i32_1);
                                            IC_ROUTINE();
                                            RESTORE_TOP (local_i32_1);
                                            break;
                                          }

    /* Addr | I64 */
    case WASM_OP_I64_STORE:
    case WASM_OP_I64_STORE8:
    case WASM_OP_I64_STORE16:
    case WASM_OP_I64_STORE32:
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
                                            SAVE_TOP (local_i64_1);
                                            IC_ROUTINE();
                                            RESTORE_TOP (local_i64_1);
                                            break;
                                          }

    /* Addr | F32 */
    case WASM_OP_F32_STORE: {
                              SAVE_TOP (local_f32);
                              IC_ROUTINE();
                              RESTORE_TOP (local_f32);
                              break;
                            }

    /* Addr | F64 */
    case WASM_OP_F64_STORE: {
                              SAVE_TOP (local_f64);
                              IC_ROUTINE();
                              RESTORE_TOP (local_f64);
                              break;
                            }

    /* Addr | I32 | I32 */
    case WASM_OP_I32_ATOMIC_RMW_CMPXCHG:
    case WASM_OP_I32_ATOMIC_RMW8_CMPXCHG_U:
    case WASM_OP_I32_ATOMIC_RMW16_CMPXCHG_U: {
                                               SAVE_TOP (local_i32_1);
                                               SAVE_TOP (local_i32_2);
                                               IC_ROUTINE();
                                               RESTORE_TOP (local_i32_2);
                                               RESTORE_TOP (local_i32_1);
                                               break;
                                             }

    /* Addr | I64 | I64 */
    case WASM_OP_I64_ATOMIC_RMW_CMPXCHG:
    case WASM_OP_I64_ATOMIC_RMW8_CMPXCHG_U:
    case WASM_OP_I64_ATOMIC_RMW16_CMPXCHG_U:
    case WASM_OP_I64_ATOMIC_RMW32_CMPXCHG_U: {
                                               SAVE_TOP (local_i64_1);
                                               SAVE_TOP (local_i64_2);
                                               IC_ROUTINE();
                                               RESTORE_TOP (local_i64_2);
                                               RESTORE_TOP (local_i64_1);
                                               break;
                                             }
  }
  return addinst;
}



/* Logstart insertion: Beginning of start function or main */
static void insert_logstart_void(WasmModule &module, FuncDecl *main_fn) {
  ImportInfo iminfo = {
    .mod_name = "instrument",
    .member_name = "logstart"
  };
  SigDecl logstart_sig = { .params = {}, .results = {} };
  ImportDecl* logstart_import_decl = module.add_import(iminfo, logstart_sig);
  FuncDecl* logstart_import = logstart_import_decl->desc.func;

  FuncDecl* start_fn = module.get_start_fn();
  FuncDecl* first_fn = start_fn ? start_fn : main_fn;
  InstList &first_insts = first_fn->instructions;
  first_insts.push_front(InstBasePtr(new CallInst(logstart_import)));
}


/* Logstart insertion: Beginning of start function or main, with global
* i32 parameter */
static void insert_logstart_global_param(WasmModule &module, FuncDecl *main_fn,
    GlobalDecl *globref) {
  ImportInfo iminfo = {
    .mod_name = "instrument",
    .member_name = "logstart"
  };
  SigDecl logstart_sig = { .params = {WASM_TYPE_I32}, .results = {} };
  ImportDecl* logstart_import_decl = module.add_import(iminfo, logstart_sig);
  FuncDecl* logstart_import = logstart_import_decl->desc.func;

  FuncDecl* start_fn = module.get_start_fn();
  FuncDecl* first_fn = start_fn ? start_fn : main_fn;
  InstList &first_insts = first_fn->instructions;
  first_insts.push_front(InstBasePtr(new CallInst(logstart_import)));
  first_insts.push_front(InstBasePtr(new GlobalGetInst(globref)));
}


/* Logend insertion: After every return in main & at end of main */
static void insert_logend(WasmModule &module, FuncDecl *main_fn) {
  ImportInfo iminfo = {
    .mod_name = "instrument",
    .member_name = "logend"
  };
  SigDecl logend_sig = { .params = {}, .results = {} };
  ImportDecl* logend_import_decl = module.add_import(iminfo, logend_sig);
  FuncDecl* logend_import = logend_import_decl->desc.func;

  InstList &main_insts = main_fn->instructions;
  for (auto institr = main_insts.begin(); institr != main_insts.end(); ++institr) {
    InstBasePtr &instruction = *institr;
    if (instruction->is(WASM_OP_RETURN)) {
      main_insts.insert(institr, InstBasePtr(new CallInst(logend_import)));
    }
  }

  auto end_inst = main_insts.end();
  auto finish_loc = std::prev(end_inst, 1);
  main_insts.insert(finish_loc, InstBasePtr(new CallInst(logend_import)));
}


/************************************************/
/* Do not actually instrument; collect number of accesses and their locations */
static std::unordered_map<InstBasePtr, uint32_t> memaccess_dry_run (WasmModule &module) {
  std::unordered_map<InstBasePtr, uint32_t> access_idx_map;
  uint32_t local_indices[7];
  uint32_t access_idx = 0;
  for (auto &func : module.Funcs()) {
    InstList &insts = func.instructions;
    for (auto institr = insts.begin(); institr != insts.end(); ++institr) {
      InstBasePtr &instruction = *institr;
      if (instruction->getImmType() == IMM_MEMARG) {
        /* Add instruction will be empty if we choose to ignore some access instructions
          * eg: atomic.notify, atomic.wait */
        InstList addinst = setup_logappend_args(institr, local_indices, NULL, access_idx);
        if (!addinst.empty()) {
          access_idx++; 
          access_idx_map[instruction] = access_idx;
        }
      }
    }
  }
  return access_idx_map;
}






static std::vector<uint32_t> read_binary_file(const std::string& filename) {
  std::ifstream input_file(filename, std::ios::binary);

  if (!input_file) {
    throw std::runtime_error("Could not open file for instrumentation\n");
  }

  // Get the file size
  input_file.seekg(0, std::ios::end);
  size_t file_size = input_file.tellg();
  input_file.seekg(0, std::ios::beg);

  size_t num_ints = file_size / sizeof(uint32_t);
  std::vector<uint32_t> integers(num_ints);

  input_file.read(reinterpret_cast<char*>(integers.data()), file_size);

  if (input_file.gcount() != file_size) {
    throw std::runtime_error("Error reading file");
  }

  return integers;
}


/************************************************/
/* Instrument filtered accesses from vector */
static void memfiltered_instrument_internal (
    WasmModule &module, 
    std::set<uint32_t> &inst_idx_filter, 
    bool insert_global = true, 
    bool no_filter = false) {

  std::cout << "Filter | Count: " << inst_idx_filter.size() << " | ";
  for (auto &i : inst_idx_filter) {
    std::cout << i << " ";
  }
  std::cout << "\n";

  auto filter_itr = inst_idx_filter.begin();

  /* Access Instrument function import */
  ImportInfo iminfo = {
    .mod_name = "instrument",
    .member_name = "logaccess"
  };
  SigDecl logaccess_sig = {
    .params = {WASM_TYPE_I32, WASM_TYPE_I32, WASM_TYPE_I32}, .results = {} };
  ImportDecl* logaccess_import_decl = module.add_import(iminfo, logaccess_sig);
  FuncDecl* logaccess_import = logaccess_import_decl->desc.func;

  /* For memory access */
  uint32_t access_idx = 1;
  for (auto &func : module.Funcs()) {
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
      // Call foreign function after memarg
      if (instruction->getImmType() == IMM_MEMARG) {
        /* Add instruction will be empty if we choose to ignore some access instructions
          * eg: atomic.notify, atomic.wait */
        if (!no_filter && (filter_itr == inst_idx_filter.end())) { break; }
        InstList addinst = setup_logappend_args(institr, local_indices, logaccess_import, access_idx);
        if (!addinst.empty()) {
          // Add if no filtering; or if index matches
          bool filter_cond = (!no_filter && (*filter_itr == access_idx));
          if (no_filter || filter_cond) {
            printf("Inserting at Access Idx: %d | Op: %s\n", access_idx, opcode_table[instruction->getOpcode()].mnemonic);
            insts.insert(institr, addinst.begin(), addinst.end());
            if (filter_cond) { filter_itr++; }
          }
          access_idx++;
        }
      }
    }
  }

  ExportDecl* main_exp = get_main_export(module);
  FuncDecl* main_fn = main_exp->desc.func;

  /* Start/End function instrument import */
  if (insert_global) {
    /* Global to store the max instruction index number */
    uint32_t max_inst_id = (no_filter ? access_idx 
                            : (inst_idx_filter.size() ? *(--inst_idx_filter.end()) : 0));
    GlobalDecl global = {
      .type = WASM_TYPE_I32,
      .is_mutable = false,
      .init_expr_bytes = INIT_EXPR (I32_CONST, max_inst_id)
    };
    GlobalDecl* max_insts_globref = module.add_global(global, "__inst_max");
    insert_logstart_global_param (module, main_fn, max_insts_globref);
  }
  else {
    insert_logstart_void (module, main_fn);
  }

  insert_logend (module, main_fn);
}



/************************************************/
/* Instrument all accesses, filtered by path if given */
void memaccess_instrument (WasmModule &module, const std::string& path) {
  std::set<uint32_t> placeholder;
  if (path.empty()) {
    memfiltered_instrument_internal (module, placeholder, true, true);
  } else {
    std::vector<uint32_t> inst_idx_filter = read_binary_file(path);
    std::set<uint32_t> inst_idx_filterset(inst_idx_filter.begin(), inst_idx_filter.end());
    memfiltered_instrument_internal (module, inst_idx_filterset);
  }
}
/************************************************/


/************************************************/
/* Instrument random (percent) set of all memory accesses
* across cluster size, filtered by path if given */
std::vector<WasmModule> memaccess_stochastic_instrument (WasmModule &module, 
    int percent, int cluster_size, const std::string& path) {

  std::vector<uint32_t> inst_idx_filter;
  if (path.empty()) {
    uint32_t num_accesses = memaccess_dry_run(module).size();
    inst_idx_filter.resize(num_accesses);
    std::iota (inst_idx_filter.begin(), inst_idx_filter.end(), 1);
  }
  else {
    inst_idx_filter = read_binary_file(path);
  }

  uint32_t num_accesses = inst_idx_filter.size();
  printf("Num accesses: %u\n", num_accesses);
  int partition_size = (num_accesses * percent) / 100;

  std::vector<WasmModule> module_set(cluster_size, module);
  for (int i = 0; i < cluster_size; i++) {
    std::set<uint32_t> partition;
    /* Get a random sample */
    std::sample(inst_idx_filter.begin(), inst_idx_filter.end(), 
                std::inserter(partition, partition.end()), partition_size,
                std::mt19937{std::random_device{}()});

    memfiltered_instrument_internal (module_set[i], partition);
  }
  return module_set;
}
/************************************************/




/************************************************/
static std::vector<std::set<uint32_t>> balanced_partition_internal (
    WasmModule &module, 
    int cluster_size, 
    std::unordered_map<InstBasePtr, uint32_t> &access_idx_map,
    std::unordered_set<uint32_t> &inst_idx_filter) {
  
  std::set sorted_idxs(inst_idx_filter.begin(), inst_idx_filter.end());
  for (auto &i : sorted_idxs) {
    std::cout << i << " ";
  }
  std::cout << "\n";

  
  std::vector<std::set<uint32_t>> partitions(cluster_size);
  std::vector<uint32_t> partition_weight(partitions.size(), 0);

  for (auto &func : module.Funcs()) {
    ScopeList scope_list = module.gen_scopes_from_instructions(&func);
    for (auto &scope : scope_list) {
      std::vector<uint32_t> scope_idxs;
      /* Get all memaccesses within scope */
      auto next_outer_subscope = scope.outer_subscopes.begin();
      for (auto institr = scope.start; institr != scope.end; ++institr) {
        InstBasePtr instptr = *institr;
        ScopeBlock* outer_subscope = *next_outer_subscope;
        /* When encountering subscope, just skip past it */
        if ( (next_outer_subscope != scope.outer_subscopes.end()) &&
              (institr == outer_subscope->start) ) {
          institr = outer_subscope->end;
          std::advance(next_outer_subscope, 1);
          continue;
        }
        /* Get indices for memaccesses in scope */
        if (instptr->getImmType() == IMM_MEMARG) {
          uint32_t idx = 
            (access_idx_map.contains(instptr) ? access_idx_map[instptr] : 0);
          if (idx && inst_idx_filter.count(idx)) {
            scope_idxs.push_back(idx);
          }
        }
      }

      uint32_t lp_size = scope_idxs.size() / cluster_size;
      uint32_t hp_size = lp_size + 1;
      uint32_t num_hp_size = scope_idxs.size() - lp_size*cluster_size;
      uint32_t num_lp_size = cluster_size - num_hp_size;
      uint32_t dl_idx = 0;

      /* Argsort partition weight */
      std::vector<int> sort_idxs(cluster_size);
      std::iota(sort_idxs.begin(), sort_idxs.end(), 0);
      std::sort(sort_idxs.begin(), sort_idxs.end(), [&](int i, int j) {
        return partition_weight[i] < partition_weight[j];
      });

      //printf("Scope | Depth: %d ; Loop Depth: %d\n", scope.depth, scope.loop_depth);
      printf("PW | ");
      for (auto &k : partition_weight) { printf("%d ", k); }
      printf("\n");
      //printf("Sort | ");
      //for (auto &k : sort_idxs) { printf("%d ", k); }
      //printf("\n\n");

      for (int i = 0; i < num_hp_size; i++) {
        partitions[sort_idxs[i]].insert(scope_idxs.begin() + dl_idx, scope_idxs.begin() + dl_idx + hp_size);
        partition_weight[sort_idxs[i]] += (hp_size * (scope.loop_depth+1));
        dl_idx += hp_size;
      }
      for (int i = num_hp_size; i < num_hp_size + num_lp_size; i++) {
        partitions[sort_idxs[i]].insert(scope_idxs.begin() + dl_idx, scope_idxs.begin() + dl_idx + lp_size);
        partition_weight[sort_idxs[i]] += (lp_size * (scope.loop_depth+1));
        dl_idx += lp_size;
      }
      //std::cout << "Scope idx: ";
      //for (auto &i : scope_idxs) {
      //  std::cout << i << " ";
      //}
      //std::cout << "\n";
      //std::cout << "Scope idx partitions: ";
      //for (auto &part : partitions) {
      //  for (auto &i : part) {
      //    std::cout << i << " ";
      //  }
      //  std::cout << " | ";
      //}
      //std::cout << "\n";
    }
  }

  return partitions;
}

/* Instrument balanced set of all memory accesses
* across cluster size, filtered by path if given 
  * Balance by splitting assignment equally within each scope */
std::vector<WasmModule> memaccess_balanced_instrument (WasmModule &module, 
    int cluster_size, const std::string& path) {

  std::unordered_map<InstBasePtr, uint32_t> access_idx_map = memaccess_dry_run (module);
  std::unordered_set<uint32_t> inst_idx_filter;
  if (path.empty()) {
    uint32_t num_accesses = access_idx_map.size();
    for (uint32_t i = 1; i <= num_accesses; i++) {
      inst_idx_filter.insert(i);
    }
  }
  else {
    std::vector<uint32_t> filter_vec = read_binary_file(path);
    for (auto &idx : filter_vec) {
      inst_idx_filter.insert(idx);
    }
  }

  uint32_t num_accesses = inst_idx_filter.size();
  printf("Num accesses: %u\n", num_accesses);
  int partition_size = (num_accesses + cluster_size - 1) / cluster_size;

  std::vector<std::set<uint32_t>> inst_idx_partitions = 
    balanced_partition_internal (module, cluster_size, access_idx_map, 
        inst_idx_filter);

  std::vector<WasmModule> module_set(cluster_size, module);
  for (int i = 0; i < cluster_size; i++) {
    memfiltered_instrument_internal (module_set[i], inst_idx_partitions[i]);
  }

  return module_set;
}
/************************************************/
