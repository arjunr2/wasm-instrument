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
#include "ir.h"
#include "instructions.h"


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

  /* Encode instrumented module */
  bytedeque bq = module.encode_module(args.outfile);
  CallInst call(func_imp->desc.func);
  return 0;
}




