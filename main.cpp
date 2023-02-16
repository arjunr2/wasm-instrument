#include <map>

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <getopt.h>
#include <math.h>
#include <limits.h>

#include "common.h"
#include "parse.h"
#include "views.h"
#include "ir.h"
#include "instructions.h"
#include "routines.h"

#define MAX(A, B) ({ ((A > B) ? (A) : (B)); })

static struct option long_options[] = {
  {"trace", no_argument,  &g_trace, 1},
  {"dis", no_argument, &g_disassemble, 1},
  {"out", required_argument, NULL, 'o'},
  {"help", no_argument, NULL, 'h'}
};

typedef struct {
  char* infile;
  char* outfile;
} args_t;

args_t parse_args(int argc, char* argv[]) {
  int opt;
  args_t args = {0};
  optind = 0;
  while ((opt = getopt_long_only(argc, argv, "o:h", long_options, NULL)) != -1) {
    switch(opt) {
      case 0: break;
      case 'o': args.outfile = strdup(optarg); break;
      case 'h':
      default:
        ERR("Usage: %s [--trace] [--out OUTFILE] <infile>\n", argv[0]);
        exit(opt != 'h');
    }
  }

  if ( ((optind + 1) != argc)) {
    ERR("Executable takes one one positional argument \"filename\"\n");
    exit(1);
  }

  if (!args.outfile) {
    ERR("Executable requires outfile \"--out\"\n");
    exit(1);
  }
  
  //printf("Trace: %d | Dis: %d\n", g_trace, g_disassemble);
  args.infile = argv[optind];
  return args;
}



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
    else if (instptr->is(WASM_OP_RETURN)) { return weight; }
    // Other instructions, weight=1
    else { weight += get_weight(instptr, module, func, func_weights); }
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

// Main function.
// Parses arguments and either runs a file with arguments.
//  -trace: enable tracing to stderr
int main(int argc, char *argv[]) {
  args_t args = parse_args(argc, argv);
    
  byte* start = NULL;
  byte* end = NULL;

  ssize_t r = load_file(args.infile, &start, &end);
  if (r < 0) {
    ERR("failed to load: %s\n", args.infile);
    return 1;
  }

  TRACE("loaded %s: %ld bytes\n", args.infile, r);
  WasmModule module = parse_bytecode(start, end);
  unload_file(&start, &end);

  /* Instrument */
  std::map<FuncDecl*, uint64_t> func_weights = all_funcs_weight_instrument (module);

  int i = 0;
  for (auto &func : module.Funcs()) {
    TRACE("Func %d: %lu\n", i++, func_weights[&func]);
  }

  /* Encode instrumented module */
  bytedeque bq = module.encode_module(args.outfile);
  return 0;
}




