#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "routines.h"
#include "views.h"
#include "instructions.h"

#define MAX(A, B) ({ ((A > B) ? (A) : (B)); })

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

  /* Insert NOP before every call in main */
  ExportDecl* main_exp = module.find_export("_start");
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
void memaccess_instrument (WasmModule &module) {
  /* Instrument function import */
  ImportInfo iminfo = {
    .mod_name = "instrument",
    .member_name = "logaccess"
  };
  SigDecl imfunc = {
    .params = {},
    .results = {}
  };
  ImportDecl* logaccess_import_decl = module.add_import(iminfo, imfunc);
  FuncDecl* logaccess_import = logaccess_import_decl->desc.func;

  iminfo.member_name = "logend";
  ImportDecl* logend_import_decl = module.add_import(iminfo, imfunc);
  FuncDecl* logend_import = logend_import_decl->desc.func;

  /* for memory access */
  for (auto &func : module.Funcs()) {
    InstList &insts = func.instructions;
    for (auto institr = insts.begin(); institr != insts.end(); ++institr) {
      InstBasePtr &instruction = *institr;
      // Call foreign function after memarg
      if (instruction->getImmType() == IMM_MEMARG) {
        auto loc = std::next(institr);
        insts.insert(loc, InstBasePtr(new CallInst(logaccess_import)));
      }
    }
  }
  /* dump the log */
  ExportDecl* main_exp = module.find_export("main");
  if (!main_exp) {
    throw std::runtime_error("Could not find export \"main\"!");
  }
  FuncDecl* main_fn = main_exp->desc.func;
  InstList &main_insts = main_fn->instructions;
  /* Insert after every return in main */
  for (auto institr = main_insts.begin(); institr != main_insts.end(); ++institr) {
    InstBasePtr &instruction = *institr;
    if (instruction->is(WASM_OP_RETURN)) {
      main_insts.insert(institr, InstBasePtr(new CallInst(logend_import)));
    }
  }
  /* Insert at the end of main */
  auto end_inst = main_insts.end();
  auto finish_loc = std::prev(end_inst, 1);
  main_insts.insert(finish_loc, InstBasePtr(new CallInst(logend_import)));

}
