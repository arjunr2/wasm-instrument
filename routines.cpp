#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "routines.h"
#include "views.h"
#include "instructions.h"

#include <climits>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <random>

#define MAX(A, B) ({ ((A > B) ? (A) : (B)); })

static ExportDecl* get_main_export(WasmModule &module) {
  /* look for either main or _start */
  ExportDecl* main_fn_exp = module.find_export("main");
  ExportDecl* start_fn_exp = module.find_export("_start");
  if (!(main_fn_exp || start_fn_exp)) {
    throw std::runtime_error("Could not find export \"main\" or \"_start\" function!");
  }
  return main_fn_exp ? main_fn_exp : start_fn_exp;
}

/************************************************/
void sample_instrument (WasmModule& module) {
  /* Global inmodule */
  GlobalDecl global = { 
    .type = WASM_TYPE_I32, 
    .is_mutable = true,
    .init_expr_bytes = INIT_EXPR (I32_CONST, 5)
  };
  module.add_global(global, "inmodule_global");

  /* Global import */
  ImportInfo iminfo = {
    .mod_name = "instrumentest",
    .member_name = "newglob"
  };
  GlobalInfo imglob = {
    .type = WASM_TYPE_EXTERNREF,
    .is_mutable = false
  };
  module.add_import(iminfo, imglob);

  /* Function import */
  iminfo.member_name = "newfunc";
  SigDecl imfunc = {
    .params = {WASM_TYPE_I32},
    .results = {WASM_TYPE_F64}
  };
  ImportDecl* func_imp = module.add_import(iminfo, imfunc);

  /* Export find */
  ExportDecl* exp = module.find_export("printf");
  if (exp == NULL) {
    TRACE("Printf not found\n");
  }

  /* Add a I32 Const + Drop before every call in main */
  ExportDecl* main_exp = get_main_export(module);
  FuncDecl* main_fn = main_exp->desc.func;
  InstList &insts = main_fn->instructions;
  for (auto institr = insts.begin(); institr != insts.end(); ++institr) {
    InstBasePtr &instruction = *institr;
    if (instruction->is(WASM_OP_CALL)) {
      insts.insert(institr, InstBasePtr(new I32ConstInst(0xDEADBEEF)));
      insts.insert(institr, InstBasePtr(new DropInst()));
    }
  }
}
/************************************************/



/************************************************/
void loop_instrument (WasmModule &module) {
  uint64_t num_loops = 0;
  for (auto &func : module.Funcs()) {
    InstList &insts = func.instructions;
    for (auto institr = insts.begin(); institr != insts.end(); ++institr) {
      InstBasePtr &instruction = *institr;
      // Increment global after loop
      if (instruction->is(WASM_OP_LOOP)) {
        GlobalDecl global = {
          .type = WASM_TYPE_I64, 
          .is_mutable = true,
          .init_expr_bytes = INIT_EXPR (I64_CONST, 0)
        };
        GlobalDecl *gref = module.add_global(global, 
                              (std::string("__slinstrument_lpcnt_") + std::to_string(num_loops++)).c_str());
        auto loc = std::next(institr);
        InstList addinst;
        addinst.push_back(InstBasePtr(new GlobalGetInst(gref)));
        addinst.push_back(InstBasePtr(new I64ConstInst(1)));
        addinst.push_back(InstBasePtr(new I64AddInst()));
        addinst.push_back(InstBasePtr(new GlobalSetInst(gref)));
        insts.insert(loc, addinst.begin(), addinst.end());
      }
    }
  }
}
/************************************************/



/************************************************/
uint64_t func_weight_instrument (WasmModule &module, FuncDecl* func, std::map<FuncDecl*, uint64_t> &func_weights);

static uint64_t get_weight (InstBasePtr instptr, WasmModule &module, FuncDecl* func, std::map<FuncDecl*, uint64_t> &func_weights) {
  switch (instptr->getImmType()) {
    // Memory accesses
    case IMM_GLOBAL:
    case IMM_LOCAL:
    case IMM_TABLE:
    case IMM_MEMARG:  {
      return 3;
    }
    // Label
    case IMM_LABEL:
    case IMM_LABELS: {
      return 2;
    }
    // Indirect call
    case IMM_SIG_TABLE: {
      return 1000;
    }
    // Direct call: Scan next function
    case IMM_FUNC: {
      std::shared_ptr<ImmFuncInst> call_inst = static_pointer_cast<ImmFuncInst>(instptr);
      FuncDecl* call_func = call_inst->getFunc();
      return func_weight_instrument (module, call_func, func_weights);
    }
    // Consts
    case IMM_I32:
    case IMM_I64:
    case IMM_F32:
    case IMM_F64: {
      return 0;
    }
    default: return 1;
  }
}


static uint64_t scope_weight_instrument (WasmModule &module, ScopeBlock* scope, FuncDecl* func, std::map<FuncDecl*, uint64_t> &func_weights) {
  uint64_t weight = 0;
  auto next_outer_subscope = scope->outer_subscopes.begin();
  for (auto institr = scope->start; institr != scope->end; ++institr) {
    InstBasePtr instptr = *institr;
    ScopeBlock* outer_subscope = *next_outer_subscope;
    TRACE("O: %s\n", opcode_table[instptr->getOpcode()].mnemonic);
    // For scope instructions, get scope weight and advance past scope if found
    if ( (next_outer_subscope != scope->outer_subscopes.end()) && 
          (institr == outer_subscope->start) ) {
      switch (outer_subscope->get_scope_type()) {
        case BLOCK:
        case LOOP:  
        case IF:  {
          uint64_t ifweight = scope_weight_instrument (module, outer_subscope, func, func_weights);
          institr = outer_subscope->end;
          std::advance(next_outer_subscope, 1);
          weight += ifweight;
          break;
        }
        // Must never encounter ELSE; handled within IFWELSE
        case IFWELSE: {
          uint64_t ifweight = scope_weight_instrument(module, outer_subscope, func, func_weights);
          outer_subscope = *std::next(next_outer_subscope);
          uint64_t elseweight = scope_weight_instrument(module, outer_subscope, func, func_weights);
          std::advance(next_outer_subscope, 2);
          weight += MAX (ifweight, elseweight);
          break;
        }
        case INVALID: { 
          ERR("Invalid scope type\n"); 
          break;
        }
      } 
    }
    // For return, just end
    else if (instptr->is(WASM_OP_RETURN)) { 
      return weight; 
    }
    // Other instructions, weight=1
    else { 
      weight += get_weight(instptr, module, func, func_weights); 
    }
  }
  TRACE("Scope weight: %lu\n", weight);
  return weight;
}


// Returns weight of the function
uint64_t func_weight_instrument (WasmModule &module, FuncDecl* func, std::map<FuncDecl*, uint64_t> &func_weights) {
  if (module.isImport(func)) {
    func_weights[func] = 10000;
  }

  else if (!func_weights.contains(func)) {
    ScopeList scope_list = module.gen_scopes_from_instructions(func);
    uint64_t func_weight = scope_weight_instrument (module, &scope_list.front(), func, func_weights);
    TRACE("<====>\n");
    func_weights[func] = func_weight;
  }
  return func_weights[func];
}

std::map<FuncDecl*, uint64_t> all_funcs_weight_instrument (WasmModule &module) {
  // Stores intermediate function weights to avoid recomputation
  std::map<FuncDecl*, uint64_t> func_weights;
  for (auto &func : module.Funcs()) {
    func_weight_instrument (module, &func, func_weights);
  }
  return func_weights;
}
/************************************************/


/************************************************/
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
/* Do not actually instrument; collect number of accesses */
uint32_t memaccess_dry_run (WasmModule &module) {
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
        if (!addinst.empty()) { access_idx++; }
      }
    }
  }
  return access_idx;
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
static void memfiltered_instrument_internal 
        (WasmModule &module, std::vector<uint32_t> &inst_idx_filter, bool insert_global = true, bool no_filter = false) {

  for (auto &i : inst_idx_filter) {
    std::cout << i << " ";
  }
  std::cout << "\n";

  std::vector<uint32_t>::iterator filter_itr = inst_idx_filter.begin();

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
    uint32_t max_inst_id = (no_filter ? access_idx : (inst_idx_filter.back() + 1));
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
/* Instrument all accesses
* Returns number of accesses that are/would be instrumented */
void memaccess_instrument (WasmModule &module) {
  std::vector<uint32_t> placeholder;
  memfiltered_instrument_internal (module, placeholder, true, true);
}
/************************************************/

void memshared_instrument (WasmModule &module, std::string path) {
  std::vector<uint32_t> inst_idx_filter = read_binary_file(path);
  memfiltered_instrument_internal (module, inst_idx_filter);
}

/************************************************/


/************************************************/
/* Instrument random (percent) set of all memory accesses
* across cluster size */
std::vector<WasmModule> memaccess_stochastic_instrument (WasmModule &module, 
    int percent, int cluster_size) {

  uint32_t num_accesses = memaccess_dry_run (module);
  printf("Num accesses: %u\n", num_accesses);
  std::vector<uint32_t> inst_idx_range(num_accesses);
  std::iota (inst_idx_range.begin(), inst_idx_range.end(), 1);
  int partition_size = (num_accesses * percent) / 100;

  std::vector<WasmModule> module_set(cluster_size, module);
  for (int i = 0; i < cluster_size; i++) {
    std::vector<uint32_t> partition;
    /* Get a random sample */
    std::sample(inst_idx_range.begin(), inst_idx_range.end(), 
                std::back_inserter(partition), partition_size,
                std::mt19937{std::random_device{}()});

    memfiltered_instrument_internal (module_set[i], partition, false);
  }
  return module_set;
}
/************************************************/

/************************************************/
/* Instrument random (percent) set of filtered accesses from path, 
* with across cluster size */
std::vector<WasmModule> memshared_stochastic_instrument (WasmModule &module, 
    std::string path, int percent, int cluster_size) {

  std::vector<uint32_t> inst_idx_filter = read_binary_file(path);
  printf("Num accesses: %lu\n", inst_idx_filter.size());
  int partition_size = inst_idx_filter.size() * percent / 100;

  std::vector<WasmModule> module_set(cluster_size, module);
  for (int i = 0; i < cluster_size; i++) {
    std::vector<uint32_t> partition;
    /* Get a random sample */
    std::sample(inst_idx_filter.begin(), inst_idx_filter.end(), 
                std::back_inserter(partition), partition_size,
                std::mt19937{std::random_device{}()});

    memfiltered_instrument_internal (module_set[i], partition);
    printf("Instrument %d done!\n", i);
  }
  return module_set;
}
/************************************************/

